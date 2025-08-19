/********************************************
 * Copyright (c) 2025 Shun/翔海 (@shun4midx) *
 * Project: Shun4miShu                      *
 * File Type: C++ file                      *
 * File: main.cpp                           *
 ****************************************** */

#include <dpp/dpp.h>
#include <dpp/user.h>
#include <dpp/cluster.h>
#include "env_parser/env_parser.h"
#include "converter/converter.h"
#include "typist/typist.h"

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

    // Button preliminaries
    typedef struct UIState {
        int page = 0;
        std::string sentence;
        std::string username;
        std::string query;

        std::vector<std::string> chunks;
        int idx = 0;

        bool use_zhuyin = true;
    } UIState;

    constexpr int WORD_SLOTS_PER_PAGE = 15; // 20 grid cells minus 5 reserved

    auto state_by_msg = std::make_shared<std::unordered_map<dpp::snowflake, UIState>>();

    // Labels
    std::vector<std::string> labels = {};

    // Builder
    auto build_keyboard = [&](dpp::snowflake channel_id, const UIState& st, dpp::snowflake user_id) {
        const bool done = (st.idx >= (int)st.chunks.size());
        std::vector<std::string> labels = done ? std::vector<std::string>{} : zhuyinToHanzi(st.chunks[st.idx]);
    
        const int page_size = WORD_SLOTS_PER_PAGE;
        const int total = (int)labels.size();
        int pages = std::max(1, (total + page_size - 1) / page_size);
        int page  = std::clamp(st.page, 0, pages - 1);
        int start = page * page_size, end = std::min(start + page_size, total);
        bool has_prev = (page > 0), has_next = (page < pages - 1);
    
        std::string header;

        if (st.use_zhuyin) {
            header = "現在正在打的注音：";
            header += done ? "(完成)" : st.chunks[st.idx];
        } else {
            header = "現在正在打的拼音：";
            header += done ? "(完成)" : zhuyin_to_pinyin(split_zhuyin(st.chunks[st.idx]))[0];
        }
        header += "（" + std::to_string(st.idx + (done ? 0 : 1)) + "/" + std::to_string(std::max(1, (int)st.chunks.size())) + "）\n";
        header += "句子：" + st.sentence + "\n";
        header += "第" + std::to_string(page + 1) + "/" + std::to_string(pages) + "頁";
    
        dpp::message msg(channel_id, header);
    
        int word_i = start;
        for (int r = 0; r < 4; ++r) {
            dpp::component row; row.set_type(dpp::cot_action_row);
            for (int c = 0; c < 5; ++c) {
                int pos = r * 5 + c + 1;
                dpp::component btn; btn.set_type(dpp::cot_button);
    
                if (pos == 16) {
                    btn.set_label("⌦").set_style(dpp::cos_danger).set_id("nav_del_" + std::to_string(user_id));
                } else if (pos == 17) {
                    btn.set_label("<").set_style(dpp::cos_success).set_id("nav_prev_" + std::to_string(user_id)).set_disabled(!has_prev);
                } else if (pos == 18) {
                    btn.set_label("␣").set_style(dpp::cos_primary).set_id("nav_space_" + std::to_string(user_id));
                } else if (pos == 19) {
                    btn.set_label(">").set_style(dpp::cos_success).set_id("nav_next_" + std::to_string(user_id)).set_disabled(!has_next);
                } else if (pos == 20) {
                    btn.set_label("送出").set_style(dpp::cos_danger).set_id("nav_enter_" + std::to_string(user_id));
                } else {
                    if (!done && word_i < end) {
                        btn.set_label(labels[word_i])
                           .set_style(dpp::cos_primary)
                           .set_id("word_" + std::to_string(word_i) + "_" + std::to_string(user_id));
                        ++word_i;
                    } else {
                        btn.set_label("[空]").set_style(dpp::cos_primary).set_id("empty_" + std::to_string(word_i++) + "_" + std::to_string(user_id));
                    }
                }
                row.add_component(btn);
            }
            msg.add_component(row);
        }
        return msg;
    };

    // Main Program
    bot.on_log(dpp::utility::cout_logger());

    bot.on_slashcommand([&](const dpp::slashcommand_t& event) {
        // ======== BASIC INFO ======= //
        if (event.command.get_command_name() == "shun_names") {
            event.reply("Shun\n翔海\nShun/翔海\nしゅう\nしゅん\nShun4mi");
        } else if (event.command.get_command_name() == "best_programming_language") {
            event.reply("當然是C++!");
        } else if (event.command.get_command_name() == "is_cpp_good") {
            event.reply("是的！");
        } else if (event.command.get_command_name() == "is_shun_good" || event.command.get_command_name() == "翔海對我好嗎？") {
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
        try {
            if (event.command.get_command_name() == "zhuyin_to_pinyin") {
                std::string zhuyin = std::get<std::string>(event.get_parameter("zhuyin"));
                event.reply(merge(zhuyin_to_pinyin(split_zhuyin(zhuyin)), " "));
            } else if (event.command.get_command_name() == "zhuyin_to_keyboard") {
                std::string zhuyin = std::get<std::string>(event.get_parameter("zhuyin"));
                event.reply(merge(zhuyin_to_qwerty(split_zhuyin(zhuyin)), ""));
            } else if (event.command.get_command_name() == "keyboard_to_zhuyin") {
                std::string query = std::get<std::string>(event.get_parameter("query"));

                if (query[query.length() - 1] != '3' && query[query.length() - 1] != '4' && query[query.length() - 1] != '6' && query[query.length() - 1] != '7') {
                    query += " ";
                }

                event.reply(merge(qwerty_to_zhuyin(split_zhuyin_qwerty(query)), ""));
            } else if (event.command.get_command_name() == "pinyin_to_zhuyin") {
                std::string pinyin = std::get<std::string>(event.get_parameter("pinyin"));
                event.reply(merge(pinyin_to_zhuyin(split_alphanumeric(pinyin)), ""));
            } else if (event.command.get_command_name() == "pinyin_to_zhuyin_keyboard") {
                std::string pinyin = std::get<std::string>(event.get_parameter("pinyin"));
                event.reply(merge(zhuyin_to_qwerty(pinyin_to_zhuyin(split_alphanumeric(pinyin))), ""));
            } else if (event.command.get_command_name() == "chinese_to_zhuyin" || event.command.get_command_name() == "chinese_to_pinyin") {
                std::string query = std::get<std::string>(event.get_parameter("query"));
                std::string output = "";

                std::vector<std::string> parsed_query = split_utf8(query);

                for (std::string& str : parsed_query) {
                    std::vector<std::string> zhuyin = hanziToZhuyin(str);

                    output += "`" + str + "`：";

                    if (zhuyin.size() == 0) {
                        output += str;
                    } else {
                        for (int i = 0; i < zhuyin.size(); ++i) {
                            std::string display = zhuyin[i];

                            if (event.command.get_command_name() == "chinese_to_pinyin") {
                                display = zhuyin_to_pinyin(split_zhuyin(display))[0];
                            }

                            output += std::to_string(i + 1) + ". " + display + "  ";
                        }
                    }

                    output += "\n";
                }

                event.reply(output);
            }
        } catch (const std::exception& e) {
            event.reply("您的入有誤（或現在「海嘯翔海」這個應用程式有問題），請重新確認輸入。");
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

        // ======== BUTTON COMMANDS ======== //
        try {
            if (event.command.get_command_name() == "type_qwerty_zhuyin" || event.command.get_command_name() == "type_zhuyin" || event.command.get_command_name() == "type_pinyin") {
                std::string query = std::get<std::string>(event.get_parameter("query"));

                std::string cmd = event.command.get_command_name();

                UIState st;
                st.username = event.command.usr.username;
                st.idx = 0;

                if (cmd == "type_qwerty_zhuyin") {
                    if (query[query.length() - 1] != '3' && query[query.length() - 1] != '4' && query[query.length() - 1] != '6' && query[query.length() - 1] != '7') {
                        query += " "; // End on a first tone
                    }
                    st.chunks = qwerty_to_zhuyin(split_zhuyin_qwerty(query));
                } else if (cmd == "type_zhuyin") {
                    st.chunks = split_zhuyin(query);
                } else if (cmd == "type_pinyin") {
                    st.chunks = pinyin_to_zhuyin(split_alphanumeric(query));
                    st.use_zhuyin = false;
                }

                dpp::snowflake user_id = event.command.usr.id;

                dpp::message first = build_keyboard(event.command.channel_id, st, user_id);
                event.reply(first, [state_by_msg, st, user_id](const dpp::confirmation_callback_t& cb) {
                    (*state_by_msg)[user_id] = st;
                    // std::cout << "Original User ID:" << user_id << std::endl;
                });
            }
        } catch (const std::exception& e) {
            event.reply("您的入有誤（或現在「海嘯翔海」這個應用程式有問題），請重新確認輸入。");
        }
    });

    // ======== BUTTON INTERACTION ======== //
    bot.on_button_click([&](const dpp::button_click_t& event) {
        try {
            std::string id = event.custom_id;

            // std::cout << "Button ID: " << id << std::endl;

            size_t first = id.find('_');
            size_t second = id.rfind('_');

            dpp::snowflake user_id = std::stoull(id.substr(second + 1));

            // std::cout << "Parsed User ID: " << user_id << std::endl;

            auto it = state_by_msg->find(user_id);
            if (it == state_by_msg->end()) {
                (*state_by_msg)[user_id] = UIState{0, "", event.command.usr.username, {}, {}, 0, true};
                it = state_by_msg->find(user_id);
            }
            UIState& st = it->second;
        
            auto update = [&](){
                dpp::message upd = build_keyboard(event.command.channel_id, st, user_id);
                event.reply(dpp::ir_update_message, upd);
            };
        
            if (event.command.usr.username != st.username) {
                event.reply(dpp::ir_channel_message_with_source,
                    dpp::message("只有 @" + st.username + " 可以用這個鍵盤喔！").set_flags(dpp::m_ephemeral));
                return;
            }
        
            if (id.rfind("word_", 0) == 0) {
                int idx = std::stoi(id.substr(5, second - 5));
                // Get fresh candidates for the current chunk
                if (st.idx < st.chunks.size()) {
                    auto labels = zhuyinToHanzi(st.chunks[st.idx]);
                    if (0 <= idx && idx < (int)labels.size()) {
                        st.sentence += labels[idx];
                        st.page = 0;
                        ++st.idx;
                    }
                }
                update();
            } else if (id.rfind("nav_prev_", 0) == 0) {
                st.page = std::max(0, st.page - 1);
                update();
            } else if (id.rfind("nav_next_", 0) == 0) {
                auto labels = (st.idx < (int)st.chunks.size()) ? zhuyinToHanzi(st.chunks[st.idx]) : std::vector<std::string>{};
                int pages = std::max(1, (int)((labels.size() + WORD_SLOTS_PER_PAGE - 1) / WORD_SLOTS_PER_PAGE));
                st.page = std::min(pages - 1, st.page + 1);
                update();
            } else if (id.rfind("nav_space_", 0) == 0) {
                st.sentence += " ";
                update();
            } else if (id.rfind("nav_del_", 0) == 0) {
                if (st.idx > 0) {
                    // Go back a chunk
                    --st.idx;
                    st.page = 0;
                }
                
                std::vector<std::string> parsed_sentence = split_utf8(st.sentence);
                parsed_sentence.pop_back();

                st.sentence = merge(parsed_sentence, "");

                update();
            } else if (id.rfind("nav_enter_", 0) == 0) {
                // Finish
                state_by_msg->erase(event.command.message_id);
                dpp::message cleared(event.command.channel_id, st.sentence);
                cleared.components.clear();
                event.reply(dpp::ir_update_message, cleared);
            } else {
                event.reply(dpp::ir_deferred_update_message, dpp::message());
            }
        } catch (const std::exception& e) {
            event.reply("您的入有誤（或現在「海嘯翔海」這個應用程式有問題），請重新確認輸入。");
        }
    });
    

    // ======== LISTENING ======== //
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

            bot.global_command_create(dpp::slashcommand("keyboard_to_zhuyin", "把QWERTY鍵盤當作注音鍵盤打，然後我就會幫你自動轉換成注音！", bot.me.id)
                .add_option(dpp::command_option(dpp::co_string, "query", "你想要「翻譯」成注音的一段話", true)));

            bot.global_command_create(dpp::slashcommand("pinyin_to_zhuyin", "打拼音，然後我就會幫你自動轉換成注音", bot.me.id)
                .add_option(dpp::command_option(dpp::co_string, "pinyin", "你想要「翻譯」成注音的一段話", true)));

            bot.global_command_create(dpp::slashcommand("pinyin_to_zhuyin_keyboard", "打拼音，然後我就可以變成，像QWERTY那樣打同一句話的注音", bot.me.id)
                .add_option(dpp::command_option(dpp::co_string, "pinyin", "你想要「翻譯」成注音QWERTY鍵盤的一段話", true)));

            bot.global_command_create(dpp::slashcommand("type_qwerty_zhuyin", "把QWERTY鍵盤當作注音鍵盤，我就可以幫你打中文！", bot.me.id)
                .add_option(dpp::command_option(dpp::co_string, "query", "你想要打的一段話", true)));

            bot.global_command_create(dpp::slashcommand("type_zhuyin", "打出注音，我就可以幫你打中文！", bot.me.id)
                .add_option(dpp::command_option(dpp::co_string, "query", "你想要打的一段話", true)));

            bot.global_command_create(dpp::slashcommand("type_pinyin", "打出拼音，我就可以幫你打中文！", bot.me.id)
                .add_option(dpp::command_option(dpp::co_string, "query", "你想要打的一段話", true)));

            bot.global_command_create(dpp::slashcommand("chinese_to_zhuyin", "把中文句字變成注音", bot.me.id)
                    .add_option(dpp::command_option(dpp::co_string, "query", "你想要變成注音的中文句字", true)));

            bot.global_command_create(dpp::slashcommand("chinese_to_pinyin", "把中文句字變成注音", bot.me.id)
                    .add_option(dpp::command_option(dpp::co_string, "query", "你想要變成拼音的中文句字", true)));
        }
    });

    bot.start(dpp::st_wait);

    return 0;
}