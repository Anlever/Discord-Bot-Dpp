#include <iostream>
#include <fstream>
#include <string>
#include "MyBot.h"
#include <dpp/dpp.h>

using namespace std;

/* Be sure to place your token in the line below.
 * Follow steps here to get a token:
 * https://dpp.dev/creating-a-bot-application.html
 * When you invite the bot, be sure to invite it with the 
 * scopes 'bot' and 'applications.commands', e.g.
 * https://discord.com/oauth2/authorize?client_id=940762342495518720&scope=bot+applications.commands&permissions=139586816064
 */



std::string token(const std::string TokenFileDirection)
{
	// Получаем текущий рабочий каталог
	std::filesystem::path currentPath = std::filesystem::current_path();

	// Поднимаемся на два уровня вверх и добавляем путь к папке "Discord Bot"
	std::filesystem::path tokenFilePath = currentPath.parent_path() / TokenFileDirection;

	std::string line;

	std::ifstream in(tokenFilePath);
	if (in.is_open())
	{
		while (std::getline(in, line))
		{
			//std::cout << line << std::endl;
		}
	}
	in.close();

	return line;
}



// Читаем токен из файла
const std::string BOT_TOKEN = token("DiscordToken.txt");



int main()
{
	setlocale(LC_ALL, "en_US.UTF-8");

	/* Create bot cluster */
	dpp::cluster bot(BOT_TOKEN);

	/* Output simple log messages to stdout */
	bot.on_log(dpp::utility::cout_logger());

	/* Register slash command here in on_ready */
	bot.on_ready([&bot](const dpp::ready_t& event) {
		if (dpp::run_once<struct register_bot_commands>()) {
			
			dpp::slashcommand pingcommand("пинг", "Пинг понг!", bot.me.id);
			dpp::slashcommand rollcommand("ролл", "Ебейший ролл кто лох!", bot.me.id);
			

			bot.guild_bulk_command_create({ pingcommand, rollcommand }, 1067805710567616512);
		}


	});

	/* Handle slash command with the most recent addition to D++ features, coroutines! */
	bot.on_slashcommand([](const dpp::slashcommand_t& event) -> dpp::task<void> {
		if (event.command.get_command_name() == "пинг") {
			co_await event.co_reply(dpp::message("Понг!").set_flags(dpp::m_ephemeral));
		}
		else if (event.command.get_command_name() == "ролл") {

			int numb = rand() % 100 + 1;

			std::string random_value = std::to_string(numb);
			if (numb == 1) {
				std::string message = "Ебать ты лох у тебя выпало " + std::to_string(numb) + "!";
				co_await event.co_reply(message);
			}
			else if (numb >= 2 && numb <= 39) {
				std::string message = "Ну такое... У тебя выпало " + std::to_string(numb) + "!";
				co_await event.co_reply(message);
			}
			else if (numb >= 40 && numb <= 80) {
				std::string message = "Ну норм у тебя выпало " + std::to_string(numb) + "!";
				co_await event.co_reply(message);
			}
			else if (numb >= 81 && numb <= 99) {
				std::string message = "Ну ваще меганормалдыр у тебя выпало " + std::to_string(numb) + "!";
				co_await event.co_reply(message);
			}
			else if (numb == 100) {
				std::string message = "Ебать ты лаки у тебя " + std::to_string(numb) + "!";
				co_await event.co_reply(message);
			}
		}
		
		
		co_return;
	});


	/* Start the bot */
	bot.start(dpp::st_wait);

	return 0;
}
