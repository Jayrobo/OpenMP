#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <iomanip>
#include <set>
#include "FourPieces.h"
#include <vector>
#include <thread>
#include <mutex>
//#include <condition_variable>


using namespace std;
//for holding the multithreaded content
vector<key_val> all_content;

struct comp
{
	//reference material http://www.techiedelight.com/sort-map-values-cpp/
	template<typename T>
	bool operator()(const T& left, const T& right) const
	{
		if (left.second != right.second)
			return left.second > right.second;
	
		return left.first > right.first;
	}
};

void First_Word_Count()
{
	ifstream input_file;
	string filename;
	multimap <string, int> content;

	do
	{
		cin >> filename;
		input_file.open(filename);

		if (input_file.fail())
			cout << "Error, please input with correct file name: ";
	} while (input_file.fail());

	int word_count = 0;
	
	while (!input_file.eof())
	{
		//getline(input_file, content);
		string temp; 
		input_file >> temp;

		for (int i = 0; i < temp.size(); ++i)
		{
			temp[i] = tolower(temp[i]); //convert everything into lower cases

			if (!((temp[i] >= 'a' && temp[i] <= 'z') || (temp[i] == '\'') || (temp[i] == '-')))
			{
				if (i == 0)
				{
					temp = temp.substr(1, temp.size());
					temp[i] = tolower(temp[i]); //since the first character has been replaced, need to make sure it's also lower cased
				}
				else
				{
					temp[i] = ':';
				}
			}
		}

		size_t found = temp.find_first_of(":");
		if (found != string::npos)
		{
			temp = temp.substr(0, found); //to make sure the word doesn't include other symbols 
		}
		
		if (temp != "\0" && temp != "-") //remove stand along character
		{
				content.insert(pair<string, int>(temp, 1));
				word_count++;
		}
			
		//cout << temp << endl;
		//content.push_back(temp.substr());
	}

	std::map<string, int> reduced_content;
	string current_content = "a";
	int similar_count = 1; //since its registered in map, it means the least count it will have is 1
	std::map<string, int>::iterator it = content.begin();
	for (it; it != content.end(); it++)
	{
		if (it->first != current_content)
		{
			if (current_content == "a") //because the first "a" will be passing into the eqaul statement while it only has 1 "a"
				reduced_content.insert(pair<string, int>(current_content, similar_count - 1));
			else
				reduced_content.insert(pair<string, int>(current_content, similar_count));

			current_content = it->first; //update for the next comparison
			similar_count = 1; //reset the count because a new key has arrived

			//cout << current_content << endl;
		}
		else if (it->first == current_content)
		{
			similar_count++; //adding one for similar count
			//cout << current_content << endl;
		}
			
	}
	//since "your" was the last key that did not get processed
	reduced_content.insert(pair<string, int>(current_content, similar_count));
	
	set<pair<string, int>, comp> set(reduced_content.begin(), reduced_content.end());
	
	//for (map<string,int>::iterator it = content.begin(); it != content.end(); it++)
	//	cout << left << setw(30) << it->first << it->second << endl;
	//for (map<string,int>::iterator it = reduced_content.begin(); it != reduced_content.end(); it++)
	//	cout << left << setw(30) << it->first << it->second << endl;
	int i = 0;

	for (auto const &pair : set) //tried to manually produce the auto type but idk how
		cout << left << setw(30) << pair.first << pair.second << endl; //<< "  " <<i++ <<endl;
	//cout << content << " " << word_count <<endl;


	input_file.close();
	//cout << word_count << endl;
}


//-----------------------------------------------------------------------------------//
//									Generic Part									 //
//-----------------------------------------------------------------------------------//
mutex locker;
int all_content_size = 0;
//condition_variable cv;
//unique_lock<mutex> Locked(locker);
bool ready = false;
void Second_Word_Count(vector<key_val> content)
{
	for (int i = 0; i < content.size(); i++)
		content[i] = mapper(content[i].key);

	int pos = 0;
	
	while (pos < content.size())
	{
		//cout << "now doing a thread" << endl;
		unique_lock<mutex> Locked(locker);
		//cout << "Locked is Created" << endl;
		if (all_content.size() == 0) //input the fist structure into the vector
		{
			//unique_lock<mutex> Locked(locker);
			//cv.wait(Locked);
			//Locked.lock();
			//cout << "Locked is DEAD if this output twice" << endl;
			all_content.push_back(key_val());
			all_content[all_content_size] = content[pos];
			all_content_size++;
			//ready = true;
			//Locked.unlock();
			//cv.notify_one();
			//cout << "First Word is Done" << endl;

			//break;
		}
		else
		{
			//cout << "Second word starts from now" << endl;
			for (int i = 0; i < all_content_size; i++)
			{
				if((i == all_content_size-1) && (all_content[i].key != content[pos].key))
				{
					//unique_lock<mutex> Locked(locker);
					//cout << "Is the lock dead???" << endl;
					//cv.wait(Locked);
					//Locked.lock();
					all_content.push_back(key_val());
					all_content[all_content_size] = content[pos];
					all_content_size++;

					//Locked.unlock();
					//cv.notify_one();
					//cout << "Proceed with second word and so on...." << endl;
					break;
				}
				else if (all_content[i].key == content[pos].key) //if there is the same key then you can break from the loop
				{	
					//unique_lock<mutex> Locked(locker);
					//cv.wait(Locked);
					//Locked.lock();
					all_content[i] = reduce(all_content[i]);
					//Locked.unlock();
					//cv.notify_one();
					//cout << "Found the same key, add them up" << endl;
					break; 
				}
				//ready = false;
			}
			
		}

		pos++; //accessing the next vector content of the thread
	}
}



int main()
{
	string method1_2;
	cout << "Please Enter 1 or 2" << endl;
	cin >> method1_2;
	
	if (method1_2 == "1")
	{
		cout << "Please input the input file name:";
		First_Word_Count();
	}
	//cout << "done";
	else if (method1_2 == "2")
	{
		vector<key_val> content;
		content = inputter("text.txt");

		//map the content
		for (int i = 0; i < content.size(); i++)
		{
			content[i] = mapper(content[i].key);
		}


		vector<key_val> content_odd, content_even;
		int num = 0;
		for (int i = 0; i <= content.size(); i = i + 2)
		{

			if (i + 1 >= content.size())
			{
				//do nothing, avoid out of memory access
			}
			else
			{
				content_odd.push_back(key_val());
				content_odd[num] = content[i + 1];
			}

			if (i >= content.size())
			{
				//do nothing, avoid out of memory access
			}
			else
			{
				content_even.push_back(key_val());
				content_even[num] = content[i];
			}

			num++;
		}

		//check if even and odd are partition properly
		/*for (int i = 0; i < content_odd.size(); i++)
		{
			cout << left << setw(30) << content_odd[i].key << "   " << content_even[i].key << endl;
		}
		cout << content_even[content_even.size()-1].key << endl;*/

		thread index_odd(Second_Word_Count, content_odd);
		thread index_even(Second_Word_Count, content_even);


		index_odd.join();
		index_even.join();
		outputter(all_content);
	}
	else
	{
		//do nothing
	}

	//cout << content_even.size() + content_odd.size() << "     " << content.size()<< endl;
	//string can be directly compared
	//if (content[5].key == content[7].key)
	//	cout << "IT WORKS with the all_content size: " << all_content.size() << "  " << all_content_size << endl;

	system("pause");
	return 0;
}