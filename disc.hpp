#include <vector>
#include <string>
#include <array>
#include <utility>
#pragma warning(disable : 4996)

enum week {
	poniedzialek = 0,
	wtorek = 1,
	sroda = 2,
	czwartek = 3,
	piatek = 4,
	sobota = 5,
	niedziela = 6
};

std::array<std::string, 7> week_inverted = { "Poniedzialek", "Wtorek", "Sroda", "Czwartek", "Piatek", "Sobota", "Niedziela" };

int zellersAlgorithm(int day, int month, int year) {
	int mon;
	if (month > 2)
		mon = month; 
	else {
		mon = (12 + month); 
		year--; 
	}
	int y = year % 100; 
	int c = year / 100; 
	int w = (day + floor((13 * (mon + 1)) / 5) + y + floor(y / 4) + floor(c / 4) + (5 * c));
	w = w % 7;
	return w;
}

int day_of_week(int add = 0) {
	std::time_t tp = std::time(NULL);   
	std::tm* ts = std::localtime(&tp); 
	int ans = zellersAlgorithm(ts->tm_mday, ts->tm_mon, 1900 + ts->tm_year) + add;
	return ans % 7;
}

std::pair<int, int> parse_time(std::string time) {
	int Hours;
	int Minutes;
	std::string s = "";
	for (auto i : time) {
		if (i == ':') {
			Hours = atoi(s.c_str());
			s.clear();
			continue;
		}
		s += i;
	}
	Minutes = atoi(s.c_str());
	return { Hours,Minutes };
}

namespace disc {
	using namespace std;

	vector<string> StartTime = { "8:00", "8:50", "9:40", "10:35", "11:30", "12:20", "13:10", "14:00", "14:50" };
	vector<string> EndTime = { "8:45", "9:35", "10:25", "11:20", "12:15", "13:05", "13:55", "14:45", "15:35" };
	vector<string> Lessons[7];
	void LessonsInit() {
		for (int i = 0; i < 7; i++)Lessons[i].resize(9);
		Lessons[poniedzialek] = { "0", "Polski", "Historia", "Matematyka", "Matematyka", "Angielski", "Religia", "Fizyka", "0" };
		Lessons[wtorek] = { "0","Angielski", "Jezyk Obcy", "Jezyk Obcy", "Informatyka", "Informatyka", "WF", "WF", "0"};
		Lessons[sroda] = { "WOS", "Polski", "Matematyka", "Historia", "Biologia", "Geografia", "Wychowawcza", "0", "0" };
		Lessons[czwartek] = { "Polski", "Polski", "Matematyka", "Religia", "Angielski", "Angielski", "0", "0", "0" };
		Lessons[piatek] = { "0", "0", "Polski", "EDB", "Chemia", "Angielski", "Angielski", "0", "0" };
		Lessons[sobota] = { "0", "0", "0", "0", "0", "0", "0", "0", "0", "0" };
		Lessons[niedziela] = { "0", "0", "0", "0", "0", "0", "0", "0", "0", "0" };
	}

	void ScheduleSend(vector<string>, SleepyDiscord::Message, bool = 0);
	void TimesSend(SleepyDiscord::Message chid);
	void ile_jeszcze_do_kurwy(SleepyDiscord::Message chid);
	void next(SleepyDiscord::Message chid);
	void help(SleepyDiscord::Message chid);

	class Niewolnik : public SleepyDiscord::DiscordClient {
	public:
		using SleepyDiscord::DiscordClient::DiscordClient;
		void onMessage(SleepyDiscord::Message message) override {
			if (message.startsWith("!plan")) {

				ScheduleSend(Lessons[day_of_week()], message);
			}
			else if (message.startsWith("!czas")) {
				TimesSend(message);
			}
			else if (message.startsWith("!jutro")) {
				ScheduleSend(Lessons[day_of_week(1)], message,1);
			}
			else if (message.startsWith("!ile jeszcze do kurwy")) {
				ile_jeszcze_do_kurwy(message);
			}
			else if (message.startsWith("!next")) {
				next(message);
			}
			else if (message.startsWith("!help")) {
				help(message);
			}
		}
	};

	Niewolnik client("", SleepyDiscord::USER_CONTROLED_THREADS);

	class embedSend {
	public:
		SleepyDiscord::Embed embed;

		void send(SleepyDiscord::Message chid) {
			embed.footer.text = "Bot ufundowany przez pierwiastek z czech!";
			embed.color = 0x4FFFB0;
			embed.footer.iconUrl = "https://cdn.discordapp.com/avatars/921493909794852934/e49449ed58cf4471a1db72fa385ef02a.webp?size=80";

			client.sendMessage(chid.channelID, "", embed);
		}
	};

	void ScheduleSend(vector<string> Today, SleepyDiscord::Message chid, bool tmrw) {
		embedSend e;

		if (tmrw)e.embed.title = "Jutro jest " + week_inverted[day_of_week(1)];
		else e.embed.title = "Dzisiaj mamy " + week_inverted[day_of_week()];

		for (int i(0); i < Today.size(); i++) {
			if (Today[i] == "0")continue;
			e.embed.description += to_string(i+1) + ". " + Today[i] + "\n";
		}

		if (e.embed.description.empty())e.embed.title = "WRESZCIE WOLNE";
		e.send(chid);
	}

	void TimesSend(SleepyDiscord::Message chid) {
		embedSend e;
		e.embed.title = "Czas";

		for (int i = 0; i < 9; i++) {
			e.embed.description += to_string(i + 1) + ". " + StartTime[i] + "-" + EndTime[i] + "\n";
		}

		e.send(chid);
	}

	void ile_jeszcze_do_kurwy(SleepyDiscord::Message chid){
		embedSend e;
		time_t currentTime;
		struct tm* localTime;

		time(&currentTime);                   // Get the current time
		localTime = localtime(&currentTime);  // Convert the current time to the local time

		int Hour = localTime->tm_hour;
		int Min = localTime->tm_min;
		
		int i = 0; bool found = false;
		for(; i < 9; i++){
			if(Hour*60+Min >= parse_time(StartTime[i]).first*60 + parse_time(StartTime[i]).second)
				if (Hour * 60 + Min <= parse_time(EndTime[i]).first * 60 + parse_time(EndTime[i]).second) { found = true; break; }
		}

		if (!found) {
			ScheduleSend(Lessons[niedziela], chid);
			return;
		}
		e.embed.title = "Aktualnie jest " + Lessons[day_of_week()][i];
		
		e.embed.description += "Do jej konca zostalo " + to_string((parse_time(EndTime[i]).first * 60 + parse_time(EndTime[i]).second) - (Hour*60+Min)) + " minut\n";
		e.embed.description += "Konczymy lekcje o ";

		int j = 8;
		for (; j >= 0; j--) {
			if (Lessons[day_of_week()][j] != "0") {
				e.embed.description += EndTime[j] + "\n\n";
				break;
			}
		}
		e.embed.description += to_string(parse_time(EndTime[j]).first - Hour) + " godzin\n";
		e.embed.description += to_string((parse_time(EndTime[j]).first * 60 + parse_time(EndTime[j]).second) - (Hour * 60 + Min)) + " minut\n";
		e.embed.description += to_string((parse_time(EndTime[j]).first * 3600 + parse_time(EndTime[j]).second * 60) - (Hour * 3600 + Min * 60)) + " sekund\n";

		e.embed.description += "do konca pierdolnika dzisiaj";
		e.send(chid);
	}

	void next(SleepyDiscord::Message chid) {
		embedSend e;
		time_t currentTime;
		struct tm* localTime;

		time(&currentTime);                   // Get the current time
		localTime = localtime(&currentTime);  // Convert the current time to the local time

		int Hour = localTime->tm_hour;
		int Min = localTime->tm_min;

		int i = 0; bool found = false;
		for (; i < 9; i++) {
			if (Hour * 60 + Min >= parse_time(StartTime[i]).first * 60 + parse_time(StartTime[i]).second)
				if (Hour * 60 + Min <= parse_time(EndTime[i]).first * 60 + parse_time(EndTime[i]).second) { found = true; break; }
		}

		if (!found) {
			ScheduleSend(Lessons[niedziela], chid);
			return;
		}
		e.embed.title = "Aktualnie jest " + Lessons[day_of_week()][i];

		e.embed.description += "Do jej konca zostalo " + to_string((parse_time(EndTime[i]).first * 60 + parse_time(EndTime[i]).second) - (Hour * 60 + Min)) + " minut\n";
		if (i == 8)e.embed.description += "Potem Koniec";
		else {
			e.embed.description+="Potem " + Lessons[day_of_week()][i+1];

		}

		e.send(chid);
	}

	void help(SleepyDiscord::Message chid) {
		embedSend e;
		e.embed.title = "Pomoc";
		e.embed.description += "!plan\n!czas\n!jutro\n!ile jeszcze o kurwy\n!next";
		e.send(chid);
	}

}