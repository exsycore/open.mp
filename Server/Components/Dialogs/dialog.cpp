/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2022, open.mp team and contributors.
 */

#include <Impl/events_impl.hpp>
#include <Server/Components/Dialogs/dialogs.hpp>
#include <netcode.hpp>
#include <tis620.hpp>

using namespace Impl;

String emptyString = " ";
StringView emptyStringView = emptyString;

static const NetCode::RPC::ShowDialog hideDialog = {
	{},
	INVALID_DIALOG_ID,
	static_cast<uint8_t>(DialogStyle_MSGBOX),
	emptyStringView,
	emptyStringView,
	emptyStringView,
	emptyStringView
};

class PlayerDialogData final : public IPlayerDialogData
{
private:
	int activeId = INVALID_DIALOG_ID;

	String title_ = "";
	String body_ = "";
	String button1_ = "";
	String button2_ = "";
	DialogStyle style_ = DialogStyle_MSGBOX;

	friend class DialogsComponent;

public:
	void hide(IPlayer& player) override
	{
		if (activeId != INVALID_DIALOG_ID)
		{
			PacketHelper::send(hideDialog, player);
			activeId = INVALID_DIALOG_ID;
		}
	}

	void show(IPlayer& player, int id, DialogStyle style, StringView title, StringView body, StringView button1, StringView button2) override
	{
		// We can enforce this more strictly in the component.  The legacy IDs
		// should only be in Pawn.
		if (id <= INVALID_DIALOG_ID || id >= MAX_DIALOG)
		{
			return;
		}

		// Limit list item length to prevent client from crashing
		if (style == DialogStyle_LIST)
		{
			auto pos = body.find("\n");
			if (pos == body.npos)
			{
				if (body.length() > 130)
				{
					return;
				}
			}
			else
			{
				if (pos > 130)
				{
					return;
				}
			}
		}

		char tis620_buffer[512];     // สำหรับ title
		char tis620_buffer2[4096];   // สำหรับ body (รายการยาว)
		char tis620_buffer3[128];    // สำหรับปุ่ม 1
		char tis620_buffer4[128];    // สำหรับปุ่ม 2

		const char* fix_title = title.data();
		const char* fix_body = body.data();
		const char* fix_button1 = button1.data();
		const char* fix_button2 = button2.data();

		tis620::utf8_to_tis620(fix_title, tis620_buffer, sizeof(tis620_buffer));
		tis620::utf8_to_tis620(fix_body, tis620_buffer2, sizeof(tis620_buffer2));
		tis620::utf8_to_tis620(fix_button1, tis620_buffer3, sizeof(tis620_buffer3));
		tis620::utf8_to_tis620(fix_button2, tis620_buffer4, sizeof(tis620_buffer4));

		style_ = style;
		title_ = String(tis620_buffer);
		body_ = String(tis620_buffer2);
		button1_ = String(tis620_buffer3);
		button2_ = String(tis620_buffer4);

		NetCode::RPC::ShowDialog showDialog;
		showDialog.ID = id;
		showDialog.Style = static_cast<uint8_t>(style);
		// We don't have to truncate title to be less than 64 client sided size limit.
		// Client won't crash or show any undefined behavior, it simply doesn't render.
		// 64 char limit is without counting embedded colors, let client handle then.
		showDialog.Title = tis620_buffer;
		showDialog.Body = tis620_buffer2;
		showDialog.FirstButton = tis620_buffer3;
		showDialog.SecondButton = tis620_buffer4;
		PacketHelper::send(showDialog, player);

		// set player's active dialog id to keep track of its validity later on response
		activeId = id;
	}

	void get(int& id, DialogStyle& style, StringView& title, StringView& body, StringView& button1, StringView& button2) override
	{
		id = activeId;
		style = style_;
		title = title_;
		body = body_;
		button1 = button1_;
		button2 = button2_;
	}

	int getActiveID() const override
	{
		return activeId;
	}

	void freeExtension() override
	{
		delete this;
	}

	void reset() override
	{
		// Could call `hide()` here, but then we'd have to store the player ID.
		activeId = INVALID_DIALOG_ID;
	}
};

class DialogsComponent final : public IDialogsComponent, public PlayerConnectEventHandler
{
private:
	ICore* core = nullptr;
	DefaultEventDispatcher<PlayerDialogEventHandler> eventDispatcher;

	struct DialogResponseHandler : public SingleNetworkInEventHandler
	{
		DialogsComponent& self;
		DialogResponseHandler(DialogsComponent& self)
			: self(self)
		{
		}

		bool onReceive(IPlayer& peer, NetworkBitStream& bs) override
		{
			NetCode::RPC::OnPlayerDialogResponse sendDialogResponse;
			if (!sendDialogResponse.read(bs))
			{
				return false;
			}

			// If the dialog id doesn't match what the server is expecting, ignore it
			PlayerDialogData* data = queryExtension<PlayerDialogData>(peer);
			if (!data || data->getActiveID() == INVALID_DIALOG_ID || data->getActiveID() != sendDialogResponse.ID)
			{
				return false;
			}

			if (sendDialogResponse.Response < 0 || sendDialogResponse.Response > 1)
			{
				return false;
			}

			if ((data->style_ == DialogStyle_PASSWORD || data->style_ == DialogStyle_INPUT || data->style_ == DialogStyle_MSGBOX) && sendDialogResponse.ListItem != -1)
			{
				return false;
			}

			if ((data->style_ == DialogStyle_LIST || data->style_ == DialogStyle_TABLIST || data->style_ == DialogStyle_TABLIST_HEADERS) && data->body_.length() > 0)
			{
				unsigned int lines = 0;

				for (unsigned int i = 0; i < data->body_.length() - 1; i++)
				{
					if (data->body_[i] == '\n')
					{
						lines++;
					}
				}

				if (data->style_ == DialogStyle_TABLIST_HEADERS && lines > 0)
				{
					lines--;
				}

				if (sendDialogResponse.ListItem < 0 || sendDialogResponse.ListItem > lines)
				{
					return false;
				}
			}

			data->activeId = INVALID_DIALOG_ID;

			self.eventDispatcher.dispatch(
				&PlayerDialogEventHandler::onDialogResponse,
				peer,
				sendDialogResponse.ID,
				static_cast<DialogResponse>(sendDialogResponse.Response),
				sendDialogResponse.ListItem,
				sendDialogResponse.Text);

			return true;
		}
	} dialogResponseHandler;

public:
	void onPlayerConnect(IPlayer& player) override
	{
		player.addExtension(new PlayerDialogData(), true);
	}

	StringView componentName() const override
	{
		return "Dialogs";
	}

	SemanticVersion componentVersion() const override
	{
		return SemanticVersion(OMP_VERSION_MAJOR, OMP_VERSION_MINOR, OMP_VERSION_PATCH, BUILD_NUMBER);
	}

	DialogsComponent()
		: dialogResponseHandler(*this)
	{
	}

	void onLoad(ICore* c) override
	{
		core = c;
		core->getPlayers().getPlayerConnectDispatcher().addEventHandler(this);
		NetCode::RPC::OnPlayerDialogResponse::addEventHandler(*core, &dialogResponseHandler);
	}

	void reset() override
	{
		for (IPlayer* player : core->getPlayers().entries())
		{
			auto data = queryExtension<IPlayerDialogData>(player);
			if (data)
			{
				data->hide(*player);
			}
		}
	}

	void free() override
	{
		delete this;
	}

	~DialogsComponent()
	{
		if (core)
		{
			core->getPlayers().getPlayerConnectDispatcher().removeEventHandler(this);
			NetCode::RPC::OnPlayerDialogResponse::removeEventHandler(*core, &dialogResponseHandler);
		}
	}

	IEventDispatcher<PlayerDialogEventHandler>& getEventDispatcher() override
	{
		return eventDispatcher;
	}
};

COMPONENT_ENTRY_POINT()
{
	return new DialogsComponent();
}
