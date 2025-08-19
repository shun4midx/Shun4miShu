# Shun4miShu
Discord bot 海嘯翔海 (Tsunami in Chinese + My Japanese name Shu), which deals with Chinese input in Zhuyin/Pinyin, and possible autocorrection with Zhuyin.

# Pronunciation
Please just pronounce it in English as "Shunami Shu", or in Chinese as the characters 海嘯翔海 directly. If you ever try to pronounce this in another language, you must have too much time in your life, so go touch some grass :)

## How do I touch grass?
1. Open your terminal
2. Type in
```cmd
touch grass
```
3. If you use VSCode, type in
```cmd
code grass
```
4. Now you have a lot of grass to flaunt to your non-coder friends about touching ~~and make them think you've never been out of the house in days~~

# Documentation
More detailed documentation will be coming soon when I have time to write it. For now, these are the steps:

0. Clone this repo and enter the folder:
```cmd
git clone https://github.com/shun4midx/Shun4miShu
cd Shun4miShu
```

1. Clone the DPP repo to build from source with CMake (If you don't have it, please check out how to install CMake):
```cmd
git clone https://github.com/brainboxdotcc/DPP
```

2. Create a `.env` file, and type in the following, but replace it with your relevant details without the <>:
```env
BOT_TOKEN=<Your bot token>
MESSAGE_PERMS=<Your username>,<Person 2 with message perms' username>,<etc>
BOT_USERNAME="<Your bot username including the discriminator>"
CONFLICTING_BOTS="<Any conflicting bot username including the discriminator>"
```

3. Type the following CMake commands (Make sure to install `pkg-config` first if you don't have it):
```cmd
mkdir build && cd build
cmake ..
cmake --build . --target shun4mishu -j
./shun4mishu
```

And your bot should run properly, if given the correct perms!

# Credit
The file `src/tsi.json` is derived [directly](https://github.com/dylandy/tobopomo.js/blob/master/data/tsi.json) from a lightweight JavaScript repo (tobopomo.js) for Pinyin/Zhuyin to Mandarin typing.

DPP is directly found via [this](https://github.com/brainboxdotcc/DPP) repo, and is used for C++ Discord bot coding.

There are no additional libraries used.