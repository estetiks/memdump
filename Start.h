#pragma once
#include "Win.h"



int Start()
{	
	std::cout << "\t\tMEM_DUMP_TOOL\n" << std::endl;
	while (true)
	{
		std::cout << "menu:" << std::endl;
		std::cout << "Enter 1 for get process-list" << std::endl;
		std::cout << "Enter 2 for dump virtual memory of process" << std::endl;
		std::cout << "Enter 3 for exit" << std::endl;
		std::cout << ">>  ";
		int choose = 0;
		std::cin >> choose;
		
		DWORD pid;
		std::string path;
		
		switch (choose)
		{
		case 1:
			std::cout << "\n\t\tProcess List" << std::endl;
			get_list_proc();
			std::cout << "\n\n";
			break;
		case 2:
			std::cout << "Enter the pid of dumping process >> ";
			std::cin >> pid;
			std::cout << "Enter the path to file >> ";
			std::cin >> path;
			dump_process_memory(pid, path);
			std::cout << "\n\n";
			break;
			
		case 3:
			return 0;

		default:
			std::cout << "Give me the correct param please" << std::endl;
			break;
		}
		

	}

	return 1;
}