#include <iostream>
using namespace std;

#ifndef FUNCS
#define FUNCS
int firstTask();
int secondTask();
#endif // FUNCS

int main()
{
	int select = 0;
	while (select != 3)
	{
		std::cout << endl << "Select the task number:" << endl;
		std::cout << "1 - First task / 2 - Second task / 3 - exit from programm" << endl;

		cin >> select;

		if (cin.fail())
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Incorrect enter" << endl;
		}
		else
		{
			switch (select)
			{
			case 1:
				firstTask();
				break;
			case 2:
				secondTask();
				break;
			case 3:
				cout << "bb" << endl;
				break;
			default:
				cout << "Number is incorrect" << endl;
				break;
			}
		}
	}

	

	

}
