#define CURL_STATICLIB
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <algorithm>
#include "MyBot.h"
#include <ctime>
#include "libbase64.h"
#include <dpp/dpp.h>
#include <curl/curl.h>
#include <properties.h>
#include <nlohmann/json.hpp>
#include <codecvt> 
#include <locale>  
#include <memory>
#include <stdexcept>
#include <array>
#include <filesystem>

using json = nlohmann::json;

using namespace std;



/* Be sure to place your token in the line below.
 * Follow steps here to get a token:
 * https://dpp.dev/creating-a-bot-application.html
 * When you invite the bot, be sure to invite it with the 
 * scopes 'bot' and 'applications.commands', e.g.
 * https://discord.com/oauth2/authorize?client_id=940762342495518720&scope=bot+applications.commands&permissions=139586816064
 */



std::string token(const std::string tokenFilePath)
{
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

std::string executePythonScript(const std::string& scriptPath, const std::string& filePath) {
	std::string command = "python " + scriptPath + " " + filePath;
	std::array<char, 128> buffer;
	std::string result;

	// Используем _popen и _pclose для Windows
	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command.c_str(), "r"), _pclose);

	if (!pipe) {
		throw std::runtime_error("_popen() failed!");
	}

	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}

	// Удаляем символ новой строки, если он есть
	if (!result.empty() && result.back() == '\n') {
		result.pop_back();
	}

	return result;
}

// Читаем токен из файла
const std::string BOT_TOKEN = token("C:\\Users\\Anlever1\\Desktop\\Discord Bot C++\\DiscordToken.txt");


// Функция обратного вызова для записи данных в строковую переменную
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
	size_t totalSize = size * nmemb;
	userp->append((char*)contents, totalSize);
	return totalSize;
}



void delete_file(string directory) {
	
	int status = remove(directory.c_str());

	
	if (status != 0) {
		perror("Error deleting file");
	}
	else {
		cout << "File successfully deleted" << endl;
	}
}

std::string decodeByte64(std::string& imgByte) {
	
	std::time_t newt = std::time(nullptr);
	char timeStr[100];

	// Форматирование даты и времени для файловой системы (без символов, как ':', '\n' и т.д.)
	std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d_%H-%M-%S", std::localtime(&newt));

	std::string destination = "C:\\Users\\Anlever1\\Desktop\\Discord Bot C++\\py\\tempBase64\\" + std::string(timeStr) + ".txt";



	std::ofstream outfile(destination);
	outfile << imgByte << std::endl;
	outfile.close();

	std::string scriptPath = "\"C:\\Users\\Anlever1\\Desktop\\Discord Bot C++\\py\\decode.py\"";
	std::string base64FilePath = "\"" + destination + "\"";

	std::string imagePath = executePythonScript(scriptPath, base64FilePath);

	

	delete_file(destination);

	return imagePath;
}

string json_writing(string serverout) {

	json otvet = json::parse(serverout.begin(), serverout.end());
	string edited_otvet = otvet["images"][0];
	edited_otvet.erase(std::remove(edited_otvet.begin(), edited_otvet.end(), '"'), edited_otvet.end());

	return edited_otvet;
}

string curl_responce_txt2img(json payload) {
	std::string payload_str = payload.dump();
	std::string serverout;

	CURL* curl;
	CURLcode responce;

	curl = curl_easy_init();

	if (curl) {
		struct curl_slist* slist1 = NULL;
		slist1 = curl_slist_append(slist1, "Content-Type: application/json");
		slist1 = curl_slist_append(slist1, "Accept: application/json");

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist1);
		curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:7860/sdapi/v1/txt2img");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload_str.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, payload_str.size());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &serverout);

		curl_easy_perform(curl);

		curl_easy_cleanup(curl);
		curl_global_cleanup();
	}

	return json_writing(serverout);
}



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
			dpp::slashcommand pingcommand("ping", "Пинг понг!", bot.me.id);
			dpp::slashcommand rollcommand("roll", "Ебейший ролл кто лох!", bot.me.id);
			dpp::slashcommand txt2img("txt2img", "Из текста генерирует изображение", bot.me.id);
			txt2img.add_option(
				dpp::command_option(dpp::co_string, "промт", ":Только на английском: Теги на основе которых будет гененировать изображение (Men, women, gem...)", true)
			);
			txt2img.add_option(
				dpp::command_option(dpp::co_integer, "ширина", "Ширина картинки в пикселях 0-1920", true)
				.set_min_value(0)
				.set_max_value(1920)
			);
			txt2img.add_option(
				dpp::command_option(dpp::co_integer, "высота", "Размер картинки в пикселях 0-1080", true)
				.set_min_value(0)
				.set_max_value(1080)
			);
			txt2img.add_option(
				dpp::command_option(dpp::co_integer, "качество", "Насколько качественна будет генерация 0-15", true)
				.set_min_value(0)
				.set_max_value(15)
			);
			dpp::slashcommand img2img("img2img", "Преображает изображение согласно тегам", bot.me.id);

			
			

			bot.guild_bulk_command_create({ pingcommand, rollcommand, txt2img, img2img }, 1067805710567616512);
		}
		


	});
	
	/* Handle slash command with the most recent addition to D++ features, coroutines! */
	bot.on_slashcommand([](const dpp::slashcommand_t& event) -> dpp::task<void> {
		if (event.command.get_command_name() == "ping") {
			co_await event.co_reply(dpp::message("Понг!").set_flags(dpp::m_ephemeral));
		}
		else if (event.command.get_command_name() == "roll") {

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
		else if (event.command.get_command_name() == "txt2img") {

			co_await event.co_reply(dpp::message(event.command.channel_id, "Идёт генерация..."));

			json payload = {
				{ "prompt", std::get<std::string>(event.get_parameter("промт")) },
				{ "width", std::get<int64_t>(event.get_parameter("ширина")) },
				{ "height", std::get<int64_t>(event.get_parameter("высота")) },
				{ "steps", std::get<int64_t>(event.get_parameter("качество")) }
			};

			

			string imgBytes = curl_responce_txt2img(payload);

			event.edit_response(dpp::message(event.command.channel_id, "Уже почти сгенерировалось..."));

			std::string img_path = decodeByte64(imgBytes);

			std::string formattedMessage = std::format(
				"Тэги: {} Размер: {} Качество: {}",
				payload["prompt"].get<std::string>(),
				payload["width"].get<int>(),
				payload["steps"].get<int>()
			);
			

			std::filesystem::path filePath(img_path);

			std::string directory = filePath.parent_path().string();

			// Получаем имя файла
			std::string fileName = filePath.filename().string();

			cout << fileName << endl;

			cout << directory << endl;

			dpp::message msg(event.command.channel_id, formattedMessage);
			msg.add_file(fileName, dpp::utility::read_file(img_path));

			dpp::embed embed;
			embed.set_image("attachment://" + fileName); /* Set the image of the embed to the attached image. */

			/* Add the embed to the message. */
			msg.add_embed(embed);

			/* Reply to the user with the message, with our file attached. */
			event.edit_response(msg);

			delete_file(img_path);
		}
		else if (event.command.get_command_name() == "img2img") {

		}


		co_return;
		});

	
	
	


	/* Start the bot */
	bot.start(dpp::st_wait);

	return 0;
}




