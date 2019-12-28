#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif


#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include "VariadicTable.h"
#include <cstdio>
#include <iostream>
#include <string>
#include <filesystem>
#include <vector>


using namespace rapidjson;
using namespace std;

typedef unsigned long long int ull;

struct ShowTimeDetail {
	string StartDate;//Which day in week
	int StartHour;
	int StartMinute;
	int Capacity;
	int Remained;
	int ShowTimeId;
	int MovieId;
};

struct CinemaSalon {
	int SalonId;
	vector<ShowTimeDetail> std;//ShowTimeDetails In a Week with date
};

struct Cinema {
	int SalonsQuantity;
	int ShowTimesQuantityInEachSalonEveryWeek;
	int ShowTimesDuration;
	vector<CinemaSalon> cs;
};

struct Movie {
	int MovieId;
	string MovieName;
	string DirectorName;
	string Genre;
	int ProductionYear;
	float Rate;
};

struct ShowTimeReserve {
	int Id;
	int ShowTimeId;
	int SalonId;
	int Remained;
	string StartDate;
	string Time;//12:45
	ull SortingFactor;
};


vector<Movie> Movies;
vector<ShowTimeReserve> SortedShowTimeReserve;
Cinema CinameDetail;
bool isCinemaSetuped, isAnyMovieAvailable;

string GetFileNameWithoutExtension(string filename) {
	const size_t period_idx = filename.rfind('.');
	if (std::string::npos != period_idx)
	{
		filename.erase(period_idx);
	}
	return filename;
}

int GetMaxIdInDirectory(string path)
{
	int maxId = 0;
	for (const auto& entry : filesystem::directory_iterator(path))
	{
		string filename = entry.path().filename().string();
		
		filename = GetFileNameWithoutExtension(filename);
		int id = stoi(filename);
		if (maxId < id)
		{
			maxId = id;
		}
	}
	return maxId;
}

void WriteJsonToFile(Document* d, string path)
{
	FILE* fp = fopen(path.c_str(), "wb");

	char writeBuffer[65536];
	FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

	Writer<FileWriteStream> writer(os);
	d->Accept(writer);

	fclose(fp);
}

bool ReadJsonFromFile(Document* d , string path)
{
	FILE* fp = fopen(path.c_str(), "rb");
	if (fp == NULL) return false;
	char readBuffer[65536];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	d->ParseStream(is);
	fclose(fp);
	return true;
}

bool InsertMovie() {

	try 
	{
		//Fields of Movie Object: movie_name , production_year , director_name ,  rate(1-5) , duration(in minutes) , genre
		string MovieName, DirectorName, Genre;
		int ProductionYear;//, Duration;
		float Rate;

		//Receiving User Input
		getline(cin, MovieName);//when loop is enabled...
		cout << "Movie Name: ";
		getline(cin, MovieName);
		cout << "Director Name: ";
		getline(cin, DirectorName);
		cout << "Genre of Movie: ";
		getline(cin, Genre);
		//cout << "Duration(in minutes):";
		//cin >> Duration;
		cout << "Rate(1-5):";
		cin >> Rate;
		cout << "Production Year:";
		cin >> ProductionYear;

		//Todo: checking for valid user input

		//Defining Movie object
		//string str = "{\"movie_name\":\""+MovieName+"\",\"director_name\":\""+DirectorName+"\",\"genre\":\""+Genre+"\",\"production_year\":"+ to_string(ProductionYear)+",\"rate\":"+to_string(Rate)+",\"duration\":"+to_string(Duration)+"}";
		string str = "{\"movie_name\":\"" + MovieName + "\",\"director_name\":\"" + DirectorName + "\",\"genre\":\"" + Genre + "\",\"production_year\":" + to_string(ProductionYear) + ",\"rate\":" + to_string(Rate) + "}";
		const char* json = str.c_str();
		Document d;
		d.Parse(json);


		//Writing Json File
		string path = "Assets/Admin/Movies/" + to_string(GetMaxIdInDirectory("Assets/Admin/Movies") + 1) + ".json";
		WriteJsonToFile(&d, path);
		cout << "Movie Inserted Successfully!" << endl;
	}
	catch (int e) {
		return false;
	}
	return true;
}

bool GetMovies() {
	int cnt = 0;
	for (const auto& entry : filesystem::directory_iterator("Assets/Admin/Movies/"))
	{
		cnt++;
		string filename = entry.path().filename().string();

		int id = stoi(GetFileNameWithoutExtension(filename));
		string path = "Assets/Admin/Movies/" + filename;
		Document d;
		ReadJsonFromFile(&d, path);

		Movie m;
		m.MovieId = id;
		m.DirectorName = d["director_name"].GetString();
		m.MovieName = d["movie_name"].GetString();
		m.Genre = d["genre"].GetString();
		m.ProductionYear = d["production_year"].GetInt();
		m.Rate = d["rate"].GetFloat();
		Movies.push_back(m);
	}
	if (cnt != 0)return true;
	else return false;
}

void ViewMovies() {

	VariadicTable<int, string, string, int> vt({ "Movie Id", "Movie Name", "Director Name", "Production Year" });
	for (int i  = 0;i < Movies.size() ;i++)
	{
		Movie m = Movies.at(i);
		vt.addRow({ m.MovieId , m.MovieName, m.DirectorName ,  m.ProductionYear });
	}
	vt.print(cout);
}

bool ValidateMovieId(int MovieId) {
	for (int i = 0; i < Movies.size(); i++) {
		if (MovieId == Movies.at(i).MovieId)
		{
			return true;
		}
	}
	return false;
}

bool IsMovieInShowTimes(int MovieId)
{
	//We are sure when this function is called cinema is already setuped
	for (int i = 0; i < CinameDetail.cs.size(); i++)//CinemaSalons
	{
		for (int j = 0; j < CinameDetail.cs[i].std.size(); j++)//ShowTimes
		{
			ShowTimeDetail std = CinameDetail.cs[i].std[j];
			if (ValidateMovieId(std.MovieId)) {
				if (std.MovieId == MovieId)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool DeleteMovie() {
	try {
		int id;
		cout << "Please Enter Movie Id: ";
		scanf(" %d", &id);

		bool flag = true;
		if (isCinemaSetuped)
		{
			if (IsMovieInShowTimes(id)) flag = false;
		}

		if (flag)
		{
			string path = "Assets/Admin/Movies/" + to_string(id) + ".json";
			if (remove(path.c_str()) != 0)
				cout << "Error happened while deleting movie" << endl << "Please make sure you enter a proper movie id." << endl
				<< "You can always check for movie ids in the movies list section" << endl;
			else
			{
				puts("Movie successfully deleted!");
				return true;
			}
		}
		else {
			puts("Movie is in a showtime and cannot be deleted");
		}
	}
	catch (int e) {
		puts("Operation failed. Please try again.");
	}
	return false;
}

bool ValidateDate(string date, string* std_date) 
{
	//e.g: 03/05/98
	try
	{
		if (date.length() != 8)return false;
		if (date[2] != date[5] || date[2] != '/')return false;
		int day = stoi(date.substr(0, 2));
		int month = stoi(date.substr(3, 2));
		int year = stoi(date.substr(6, 2));
		if (day < 0 || day > 30) return false;
		if (month < 0 || month > 12) return false;
		*std_date = date;
		return true;
	}
	catch (int e) 
	{
		return false;
	}
}

bool ValidateTime(string time , int *std_hour , int *std_minute) {
	if (time.find(':') == -1) return false;
	if (time.find_first_of(':') != time.find_last_of(':'))return false;
	try
	{
		int colon_index = time.find(':');
		int hour = stoi(time.substr(0, colon_index));
		int minute = stoi(time.substr(colon_index + 1, time.length() - colon_index));
		if (hour < 15 || hour > 20) return false;//the period between 15:00 and 20:59 is the rule of the cinema
		if (minute < 0 || minute > 59) return false;

		*std_hour = hour;
		*std_minute = minute;
		return true;
	}
	catch (int e)
	{
		return false;
	}
}

bool HaveTimeConflict(int startHour, int startMinute , string startDate, vector<ShowTimeDetail> std, int duration) {
	for (int i = 0; i < std.size(); i++) {

		if (std[i].StartDate == startDate)//Date of show times should match if a conflict wants to happen
		{
			//Todo:This function still has a bug(corner case)
			int old_start = (std[i].StartHour * 60) + std[i].StartMinute;
			int old_end = old_start + duration;
			int new_start = (startHour * 60) + startMinute;
			int new_end = new_start + duration;

			if (new_start >= old_end || new_end <= old_start) {/*No Conflict*/}
			else return true;
		}
	}
	return false;
}

//bool ReadCinemaSetup() {
//	FILE* p;
//	p = fopen("Setup.txt", "rb");
//	if (p == NULL) return false;
//	fread(&CinameDetail, sizeof(Cinema), 1, p);
//	fclose(p);
//	return true;
//}
//
//void WriteCinemaSetup(Cinema c) {
//	FILE* p;
//	p = fopen("Setup.txt", "wb");
//	fwrite(&c, sizeof(Cinema), 1, p);
//	fclose(p);
//
//	ReadCinemaSetup();
//}

void HandyReplace(string* inp, string find, string replace)
{
	//while(inp->find(find) != string::npos)
		*inp = inp->replace(inp->find(find), find.length(), replace);
}

bool ReadCinemaSetup() {
	CinameDetail.cs.clear();
	Document d;
	string path = "Assets/Admin/Setup/setup.json";
	if (!ReadJsonFromFile(&d, path))return false;
	else 
	{
		CinameDetail.SalonsQuantity = d["SalonsQuantity"].GetInt();
		CinameDetail.ShowTimesQuantityInEachSalonEveryWeek = d["ShowTimesQuantityInEachSalonEveryWeek"].GetInt();
		CinameDetail.ShowTimesDuration = d["ShowTimesDuration"].GetInt();

		for (auto& v : d["CinemaSalon"].GetArray())
		{
			CinemaSalon cs;
			cs.SalonId = v["SalonId"].GetInt();
			for (auto& w : v["ShowTimeDetail"].GetArray())
			{
				ShowTimeDetail std;
				std.StartDate = w["StartDate"].GetString();
				std.StartHour = w["StartHour"].GetInt();
				std.StartMinute = w["StartMinute"].GetInt();
				std.Capacity = w["Capacity"].GetInt();
				std.Remained = w["Remained"].GetInt();
				std.ShowTimeId = w["ShowTimeId"].GetInt();
				std.MovieId = w["MovieId"].GetInt();

				cs.std.push_back(std);
			}
			CinameDetail.cs.push_back(cs);
		}
	}
	return true;
}

void WriteCinemaSetup(Cinema c) {
	const char* CinemaJson = R""""(
{
  "SalonsQuantity":{0},
  "ShowTimesQuantityInEachSalonEveryWeek":{1},
  "ShowTimesDuration":{2},
  "CinemaSalon":[
    {*}
    ]
}
)"""";
	string CinemaJsonStr(CinemaJson);

	HandyReplace(&CinemaJsonStr, "{0}", to_string(c.SalonsQuantity));
	HandyReplace(&CinemaJsonStr, "{1}", to_string(c.ShowTimesQuantityInEachSalonEveryWeek));
	HandyReplace(&CinemaJsonStr, "{2}", to_string(c.ShowTimesDuration));
	for (int i = 0; i < c.cs.size();i++)
	{
		const char* SalonJson = R""""(
{
      "SalonId":{0},
      "ShowTimeDetail":[
        {**}
      ]
}
)"""";

		string SalonJsonStr(SalonJson);

		HandyReplace(&SalonJsonStr, "{0}", to_string(c.cs[i].SalonId));
		

		for (int j = 0; j < c.cs[i].std.size(); j++) 
		{
			const char* ShowTimeJson = R""""(
{
        "StartDate":"{1}",
        "StartHour":{2},
        "StartMinute":{3},
        "Capacity":{4},
        "Remained":{5},
        "ShowTimeId":{6},
        "MovieId":{7}
}
)"""";

			string ShowTimeJsonStr(ShowTimeJson);

			HandyReplace(&ShowTimeJsonStr, "{1}", c.cs[i].std[j].StartDate);
			HandyReplace(&ShowTimeJsonStr, "{2}", to_string(c.cs[i].std[j].StartHour));
			HandyReplace(&ShowTimeJsonStr, "{3}", to_string(c.cs[i].std[j].StartMinute));
			HandyReplace(&ShowTimeJsonStr, "{4}", to_string(c.cs[i].std[j].Capacity));
			HandyReplace(&ShowTimeJsonStr, "{5}", to_string(c.cs[i].std[j].Remained));
			HandyReplace(&ShowTimeJsonStr, "{6}", to_string(c.cs[i].std[j].ShowTimeId));
			HandyReplace(&ShowTimeJsonStr, "{7}", to_string(c.cs[i].std[j].MovieId));

			HandyReplace(&SalonJsonStr, "{**}", ShowTimeJsonStr + ",{**}");

		}
		HandyReplace(&SalonJsonStr, ",{**}", "");


		HandyReplace(&CinemaJsonStr, "{*}", SalonJsonStr + ",{*}");
	}
	HandyReplace(&CinemaJsonStr, ",{*}", "");

	Document d;
	d.Parse(CinemaJsonStr.c_str());

	WriteJsonToFile(&d, "Assets/Admin/Setup/setup.json");
	ReadCinemaSetup();
}

bool CinemaSetup() {
		//Admin should setup his cinema every week
		Cinema c;

		try
		{
			string trash;//When loop is enabled
			getline(cin, trash);

			int temp;
			//Todo: make sure user input is in integer format
			
			while (1)
			{
				cout << "Please enter the number of salons of your cinema: ";
				cin >> temp;

				if (temp < 0)
				{
					cout << "Illogical input" << endl;
				}
				else 
				{
					c.SalonsQuantity = temp;
					break;
				}
			}

			while (1)
			{
				cout << "Please enter the number of show times that you want to have in every SALON each WEEK: ";
				cin >> temp;

				if (temp < 0)
				{
					cout << "Illogical input" << endl;
				}
				else
				{
					c.ShowTimesQuantityInEachSalonEveryWeek = temp;
					break;
				}
			}


			while (1)
			{
				cout << "Please enter the duration of each show time in minutes(<180): ";
				cin >> temp;

					if (temp * c.ShowTimesQuantityInEachSalonEveryWeek > 7 * 24 * 60 || temp * c.ShowTimesQuantityInEachSalonEveryWeek <= 0 || temp > 180)
					{
						cout << "Illogical input. Please Try Again." << endl;
					}
					else
					{
						c.ShowTimesDuration = temp;
						break;
					}
			}
			
				bool status = true;
				for (int i = 0; i < c.SalonsQuantity; i++)
				{
					cout << "#Salon" << i + 1 << endl;
					CinemaSalon cs;
					cs.SalonId = i + 1;
					//Fill details of all the movies that are going to be shown in the following week
					for (int j = 0; j < c.ShowTimesQuantityInEachSalonEveryWeek; j++)
					{
						ShowTimeDetail std;
						std.ShowTimeId = j + 1;
						cout << "#ShowTime" << j + 1 << endl;
						

						string date;
						cout << "Please enter the exact date in the FOLLOWING WEEK that the movie will be played(e.g: 03/05/98): ";
						cin >> date;
						if (!ValidateDate(date, &std.StartDate))
						{
							status = false;
							break;
						}


						a:
						string time;
						cout << "Attention! Show time starts only during 15:00 until 20:59" << endl;
						cout << "Please enter the exact time that the movie will be played(e.g: 20:45): ";
						cin >> time;
						if (!ValidateTime(time, &std.StartHour, &std.StartMinute))
						{
							status = false;
							break;
						}
						if (cs.std.size() > 0) {
							if (HaveTimeConflict(std.StartHour, std.StartMinute, std.StartDate, cs.std, c.ShowTimesDuration))
							{
								cout << "The entered time has conflit with previous showtimes." << endl;
								goto a;
							}
						}

						cout << "Please enter movie id from the list: ";
						cin >> std.MovieId;
						if (!ValidateMovieId(std.MovieId))
						{
							status = false;
							break;
						}



						cout << "Please specify a capacity for the show time: ";
						cin >> std.Capacity;

						std.Remained = std.Capacity;
						
						cs.std.push_back(std);
					}
					if (!status) {
						break;
					}
					else {
						c.cs.push_back(cs);
					}
				}
				if (!status) {
					cout << "Illogical input. Please Try Again." << endl;
				}
				else {
					WriteCinemaSetup(c);
					cout << "Cinema Setup is Done!" << endl;
					return true;
				}
			
		}
		catch (int e) {
			cout << "Exception happened. Please try again" << endl;
		}
		return false;
}

/* This function takes last element as pivot, places
  the pivot element at its correct position in sorted
  array, and places all smaller (smaller than pivot)
  to left of pivot and all greater elements to right
  of pivot */
int partition(ull arr[], int low, int high)
{
	ull pivot = arr[high]; // pivot 
	int i = (low - 1); // Index of smaller element 

	for (int j = low; j <= high - 1; j++) {

		// If current element is smaller than or 
		// equal to pivot 
		if (arr[j] <= pivot) {

			i++; // increment index of smaller element 
			swap(arr[i], arr[j]);
		}
	}
	swap(arr[i + 1], arr[high]);
	return (i + 1);
}

// Generates Random Pivot, swaps pivot with 
// end element and calls the partition function 
int partition_r(ull arr[], int low, int high)
{
	// Generate a random number in between 
	// low .. high 
	srand(time(NULL));
	int random = low + rand() % (high - low);

	// Swap A[random] with A[high] 
	swap(arr[random], arr[high]);

	return partition(arr, low, high);
}

/* The main function that implements QuickSort
arr[] --> Array to be sorted,
low --> Starting index,
high --> Ending index */
void RandomQuickSort(ull arr[], int low, int high)
{
	if (low < high) {

		/* pi is partitioning index, arr[p] is now
		at right place */
		int pi = partition_r(arr, low, high);

		// Separately sort elements before 
		// partition and after partition 
		RandomQuickSort(arr, low, pi - 1);
		RandomQuickSort(arr, pi + 1, high);
	}
}


void ShowTimeListSetup(int movieId) {
	//ShowTimeId<=>TimeToSort(99-12-30-1499)
	vector<ShowTimeReserve> vec_rts;
	ull SortingFactors[1000];
	int cnt = 1;

	VariadicTable<int, int, int, string , string > vt({ "Id" , "Salon" , "Remained Seats" , "Date" , "Time" });

	for (int i = 0; i < CinameDetail.cs.size(); i++) 
	{
		int salonId = CinameDetail.cs[i].SalonId;
		for (int j = 0; j < CinameDetail.cs[i].std.size(); j++)
		{
			int Id = CinameDetail.cs[i].std[j].MovieId;
			if (Id == movieId)
			{
				ShowTimeDetail std = CinameDetail.cs[i].std[j];
				ShowTimeReserve rts;
				rts.Id = cnt;
				rts.ShowTimeId = std.ShowTimeId;
				rts.SalonId = CinameDetail.cs[i].SalonId;
				rts.Remained = std.Remained;
				rts.StartDate = std.StartDate;

				string hour = to_string(std.StartHour);
				string minute = to_string(std.StartMinute);
				if (std.StartMinute < 10)
				{
					minute = "0" + to_string(std.StartMinute);
				}
				if (std.StartHour < 10)
				{
					hour = "0" + to_string(std.StartHour);
				}
				rts.Time = hour + ":" + minute;

				string date = std.StartDate;
				string day = date.substr(0, 2);
				string month = date.substr(3, 2);
				string year = date.substr(6, 2);
				rts.SortingFactor = stoull(year + month + day + hour + minute);
				SortingFactors[cnt - 1] = rts.SortingFactor;
				vec_rts.push_back(rts);


				cnt++;
			}
		}

	}
	RandomQuickSort(SortingFactors, 0, vec_rts.size());

	int sz = vec_rts.size();
	for (int i = 0; i < sz;i++)
	{
		for (int j = 0; j < vec_rts.size(); j++)
		{
			if (SortingFactors[i] == vec_rts[j].SortingFactor)
			{
				ShowTimeReserve rts = vec_rts[j];
				vt.addRow({ rts.Id , rts.SalonId , rts.Remained , rts.StartDate , rts.Time });
				SortedShowTimeReserve.push_back(vec_rts[j]);
				vec_rts.erase(vec_rts.begin() + j);
				break;
			}
		}
	}

	vt.print(cout);
}

bool ApplyReservation(int Id)
{

	//Find suitable table row from the reservation table that was show to the user
	ShowTimeReserve srt;
	for (int i = 0; i < SortedShowTimeReserve.size(); i++)
	{
		if (SortedShowTimeReserve[i].Id == Id)
		{
			srt = SortedShowTimeReserve[i];
			break;
		}
	}

	for (int i = 0; i < CinameDetail.cs.size(); i++) 
	{
		for (int j = 0; j < CinameDetail.cs[i].std.size(); j++) 
		{
			if (CinameDetail.cs[i].std[j].ShowTimeId == srt.ShowTimeId && 
				CinameDetail.cs[i].SalonId == srt.SalonId)
			{
				if (CinameDetail.cs[i].std[j].Remained > 0)
				{
					CinameDetail.cs[i].std[j].Remained--;
					WriteCinemaSetup(CinameDetail);
					return true;
				}
				else 
				{
					return false;
				}
			}
		}
	}
	return false;
}

int main() {

	isCinemaSetuped = ReadCinemaSetup();
	isAnyMovieAvailable = GetMovies();

	while (true)
	{
		char choice;
		cout << "Hi," << endl;
		cout << "Welcome to the Cinema Ticket Reservation System!" << endl;
		cout << "For Entering As Admin Press (a) Otherwise Press (u): ";
		choice = getchar();
		choice = tolower(choice);
		switch (choice)
		{
		case 'a':
			//Phase1

			while (true) 
			{
				char choice;
				string inp;
				cout << "(a) Insert Movie" << endl;
				cout << "(b) View Movies List" << endl;
				cout << "(c) Delete Movie by Id" << endl;
				cout << "(d) Define Show Times & Cinema Salons" << endl;
				cout << "(z) Back to admin panel" << endl;
				cout << "(x) Exit admin panel" << endl;
				cout << "Please, choose one of the above tasks:";
				scanf(" %c", &choice);
				choice = tolower(choice);

				bool flag = false;
				switch (choice)
				{
				case 'a':
					if (InsertMovie())
					{
						Movies.clear();
						isAnyMovieAvailable = GetMovies();
					}
					break;
				case 'b':
					if (isAnyMovieAvailable)
					{
						ViewMovies();
					}
					else {
						puts("No movies available!");
					}
					break;
				case 'c':
					if (isAnyMovieAvailable)
					{
						ViewMovies();
						if (DeleteMovie())
						{
							Movies.clear();
							isAnyMovieAvailable = GetMovies();
						}
					}
					else {
						puts("You have not defined any movie yet!");
					}
					break;
				case 'd':
					if (!isAnyMovieAvailable)
					{
						cout << "Please first define some movies for the cinema then try to define showtimes for movies!" << endl;
					}
					else 
					{
						ViewMovies();
						if (CinemaSetup())
						{
							isCinemaSetuped = true;
						}
					}
					break;
				case 'e':
					break;
				case 'z'://back to admin panel
					system("CLS");
					break;
				case 'x'://exit admin panel
					system("CLS");
					flag = true;
					break;
				default://back to admin panel
					system("CLS");
					break;
				}
				if (flag) {
					break;
				}
			}

			break;
		case 'u':
			//Phase2
			if (isAnyMovieAvailable && isCinemaSetuped) 
			{
				int movieId;
				ViewMovies();
				cout << "Please specify id of the movie that you want to reserve: ";
				cin >> movieId;
				ShowTimeListSetup(movieId);
				int Id;
				cout << "Please choose a number from id column: ";
				cin >> Id;
				if (Id < 0 || Id > SortedShowTimeReserve.size())
				{
					cout << "Invalid Id" << endl;
				}
				else
				{
					if (ApplyReservation(Id))
					{
						cout << "Reservation Succeeded" << endl;

					}
					else
					{
						cout << "Reservation failed due to lack of seats!" << endl;
					}
				}
				string trash = "";
				cout << "Press any key to continue..." << endl;
				getline(cin, trash);
				getline(cin, trash);
			}
			else 
			{
				cout << "Reservation system is not available yet. Until admin defines movies and showtimes" << endl;
			}
			break;
		default:
			system("CLS");
			break;
		}
	}
}