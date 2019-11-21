#include "commons.h"

int main()
{
	std::map<uint32_t, std::map<uint32_t, uint32_t>> result;
	string fifo_name = "fifo/voter-fifo";

	for (int number_of_fifos = 0; ; ++number_of_fifos)
	{
		string this_fifo_name = fifo_name + std::to_string(number_of_fifos);
		std::ifstream fifo_file(this_fifo_name);

		if (!fifo_file)
			break;

		while (true)
		{
			string number, detected_class;

			if (!std::getline(fifo_file, number, ','))
				break;

			std::getline(fifo_file, detected_class, '\n');
			result[stoul(number)][stoul(detected_class)] += 1;
		}

		unlink(this_fifo_name.c_str());
	}

	uint32_t result_size = result.size();
	const char* ensemble_fifo_name = "fifo/ensemble-fifo";
	int fd = open(ensemble_fifo_name, O_WRONLY | O_RDONLY | O_CREAT, 0666);
	close(fd);
	mkfifo(ensemble_fifo_name,  0666);
	fd = open(ensemble_fifo_name, O_WRONLY);

	for (uint32_t i = 0; i < result_size; ++i)
	{
		int detection = 0;
		int votes = 0;

		for (auto const& element : result.at(i))
		{
			if (votes < element.second)
			{
				votes = element.second;
				detection = element.first;
			}
		}	

		string data = std::to_string(i) + "," + std::to_string(detection) + "\n";
		write(fd, data.c_str(), data.length());
	}

	close(fd);
}