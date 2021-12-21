#include <iostream>
#include <vector>
#include <sleepy_discord/sleepy_discord.h>
#include "disc.hpp"

using namespace std;

int main() {
	disc::LessonsInit();
	disc::client.setIntents(SleepyDiscord::Intent::SERVER_MESSAGES);
	disc::client.run();
}