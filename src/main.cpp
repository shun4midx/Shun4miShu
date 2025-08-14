/********************************************
 * Copyright (c) 2025 Shun/翔海 (@shun4midx) *
 * Project: FQ-HLL-Bot                      *
 * File Type: C++ file                      *
 * File: main.cpp                           *
 ****************************************** */

#include <FQ-HLL/FQ-HLL.h>
#include <dpp/dpp.h>
#include <dpp/user.h>
#include <dpp/cluster.h>
#include "env_parser/env_parser.h"
#include "converter/converter.h"

#include <iostream>
#include <format>
#include <cstdio>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <random>
#include <ctime>
#include <cstdlib>
#include <regex>

int main() {
    std::srand(unsigned(time(NULL)));

    // Env
    static std::unordered_map<std::string, std::vector<std::string>> env = parseEnvFile();
    static std::string BOT_TOKEN = env["BOT_TOKEN"][0];
    static std::vector<std::string> MESSAGE_PERMS = env["MESSAGE_PERMS"];
    static std::string BOT_USERNAME = env["BOT_USERNAME"][0];
    static std::vector<std::string> CONFLICTING_BOTS = env["CONFLICTING_BOTS"];

    static bool correct_messages = true;

    dpp::cluster bot(BOT_TOKEN, dpp::i_default_intents | dpp::i_message_content);

    // Main Program
    bot.on_log(dpp::utility::cout_logger());

    bot.on_slashcommand([](const dpp::slashcommand_t& event) {
        // ======== BASIC INFO ======= //
        if (event.command.get_command_name() == "shun_names") {
            event.reply("Shun\n翔海\nShun/翔海\nしゅう\nしゅん\nShun4mi");
        } else if (event.command.get_command_name() == "best_programming_language") {
            event.reply("當然是C++!");
        } else if (event.command.get_command_name() == "is_cpp_good") {
            event.reply("是的！");
        } else if (event.command.get_command_name() == "is_shun_good") {
            event.reply("翔海對我很好！ *(他逼我講的 但他一直虐待我 請你救救我 :sob::sob:*");
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            event.edit_original_response(dpp::message("嗯！翔海對我超級好！:D"));
        } else if (event.command.get_command_name() == "do_i_speak_japanese") {
            event.reply("にほんごをはなせない QAQ *（ウワーン 助けて〜 翔海は「海嘯翔海」に日本語を教えたくない* :pensive:");
        } else if (event.command.get_command_name() == "do_i_speak_mandarin") {
            event.reply("當然會啦 不然為什麼幫還你打中文呢");
        } else if (event.command.get_command_name() == "can_i_do_cangjie" || event.command.get_command_name() == "can_i_do_boshiamy") {
            event.reply("媽的 當然不會啦 幹嘛要我學這些胡說八道的輸入法 :index_pointing_at_the_viewer::clown::index_pointing_at_the_viewer:");
        } else if (event.command.get_command_name() == "shun4mishu") {
            event.reply("<:shun4mishu:1405236012145442816>");
        }

        // ======== MAIN FUNCTIONS ======== //
        if (event.command.get_command_name() == "zhuyin_to_pinyin") {
            std::string zhuyin = std::get<std::string>(event.get_parameter("zhuyin"));
            event.reply(merge(zhuyin_to_pinyin(split_zhuyin(zhuyin)), " "));
        } else if (event.command.get_command_name() == "zhuyin_to_keyboard") {
            std::string zhuyin = std::get<std::string>(event.get_parameter("zhuyin"));
            event.reply(merge(zhuyin_to_qwerty(split_zhuyin(zhuyin)), ""));
        } else if (event.command.get_command_name() == "pinyin_to_zhuyin") {
            std::string pinyin = std::get<std::string>(event.get_parameter("pinyin"));
            event.reply(merge(pinyin_to_zhuyin(split_alphanumeric(pinyin)), " "));
        } else if (event.command.get_command_name() == "pinyin_to_zhuyin_keyboard") {
            std::string pinyin = std::get<std::string>(event.get_parameter("pinyin"));
            event.reply(merge(zhuyin_to_qwerty(pinyin_to_zhuyin(split_alphanumeric(pinyin))), ""));
        }
        
        // ======== USER INTERACTION ======== //
        if (event.command.get_command_name() == "say_hi") {
            // Figure out the discriminator if applicable
            std::string full_username = "";
            std::string discriminator_str = "";
            if (event.command.usr.discriminator != (uint16_t)(0)) {
                discriminator_str = std::to_string(event.command.usr.discriminator);
                while (discriminator_str.length() < 4) {
                    discriminator_str = "0" + discriminator_str;
                }
                full_username = event.command.usr.username + "#" + discriminator_str; // Format full username
            } else {
                full_username = "@" + event.command.usr.username; // Add @ to username
            }

            event.reply("嗨，**" + event.command.usr.global_name + "（" + full_username + "）**" + "！");
        }
        
    });

    // ========= LISTENING ======== //
    bot.on_message_create([&bot](const dpp::message_create_t& event) {
        // ======== MY SPECIAL FUNCTIONS ======== //
        // Carry on only if it's not from the bot itself
        if (event.msg.author.format_username() != BOT_USERNAME && !contains(CONFLICTING_BOTS, event.msg.author.format_username())) {
            std::string message = event.msg.content;

            // ======== Shun4miShu Says (My function only) ======= //
            if (message.find("!bot_say") == 0 && contains(MESSAGE_PERMS, event.msg.author.username)) {
                std::string command = "!bot_say";
                std::string quote = message.substr(command.length() + 1, message.length() - command.length());
                bot.message_delete(event.msg.id, event.msg.channel_id);
                event.send(quote);
            }

            // ======== SPECIAL FUNCTIONS ======== //

            // ======== AUTORESPONDER ======== //
            std::string og_message = message;
            std::transform(og_message.begin(), og_message.end(), og_message.begin(), ::tolower);
            if (og_message.find("shun4mishu") != std::string::npos || og_message.find("shun4mi") != std::string::npos || og_message.find("tsunami") != std::string::npos || og_message.find("shun4mi") != std::string::npos || og_message.find("海嘯翔海") != std::string::npos || og_message.find("海嘯") != std::string::npos) {
                event.reply("哇！居然被你們提到我了！:D 我很喜歡海嘯！ <:shun4mishu:1405236012145442816><:shun4mishu:1405236012145442816>", true);
            }

            // ======== AUTOCORRECT MESSAGES ======== //
            
        }
    });

    // ======== INIT PART OF THE CODE ======== //
    bot.on_ready([&bot](const dpp::ready_t& event) {
        // Bot status
        bot.set_presence(dpp::presence(dpp::ps_idle, dpp::at_game, "嘗試不被翔海虐待！"));

        if (dpp::run_once<struct register_bot_commands>()) {
            bot.global_command_create(dpp::slashcommand("shun_names", "講出Shun所有的名字", bot.me.id));
            bot.global_command_create(dpp::slashcommand("best_programming_language", "最好的程式語言是什麼呢？", bot.me.id));
            bot.global_command_create(dpp::slashcommand("is_cpp_good", "C++是個好的語言嗎？", bot.me.id));
            bot.global_command_create(dpp::slashcommand("is_shun_good", "翔海對我好嗎？", bot.me.id));
            bot.global_command_create(dpp::slashcommand("do_i_speak_japanese", "我會講日文嗎？", bot.me.id));
            bot.global_command_create(dpp::slashcommand("do_i_speak_mandarin", "我會講國語嗎？", bot.me.id));
            bot.global_command_create(dpp::slashcommand("can_i_do_cangjie", "我會倉頡輸入法嗎？", bot.me.id));
            bot.global_command_create(dpp::slashcommand("can_i_do_boshiamy", "我會嘸蝦米輸入法嗎？", bot.me.id));
            bot.global_command_create(dpp::slashcommand("shun4mishu", "把我的頭貼當作emoji來傳", bot.me.id));
            
            bot.global_command_create(dpp::slashcommand("say_hi", "我會對你說聲嗨！", bot.me.id));

            bot.global_command_create(dpp::slashcommand("zhuyin_to_pinyin", "打注音，然後我就會幫你自動轉換成拼音", bot.me.id)
                .add_option(dpp::command_option(dpp::co_string, "zhuyin", "你想要「翻譯」成拼音的一段話", true)));

            bot.global_command_create(dpp::slashcommand("zhuyin_to_keyboard", "打注音，然後我就會幫你自動轉換成，能用QWERTY鍵盤打的一段話", bot.me.id)
                .add_option(dpp::command_option(dpp::co_string, "zhuyin", "你想要「翻譯」成QWERTY鍵盤的一段話", true)));

            bot.global_command_create(dpp::slashcommand("pinyin_to_zhuyin", "打拼音，然後我就會幫你自動轉換成注音", bot.me.id)
                .add_option(dpp::command_option(dpp::co_string, "pinyin", "你想要「翻譯」成注音的一段話", true)));

            bot.global_command_create(dpp::slashcommand("pinyin_to_zhuyin_keyboard", "打拼音，然後我就可以變成，像QWERTY那樣打同一句話的注音", bot.me.id)
                .add_option(dpp::command_option(dpp::co_string, "pinyin", "你想要「翻譯」成注音QWERTY鍵盤的一段話", true)));
        }
    });

    bot.start(dpp::st_wait);

    return 0;
}