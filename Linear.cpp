#include "commons.h"

int main(int argc, char** argv)
{
	constexpr uint16_t MAX_LINE = 300;
	char message[MAX_LINE];
	int read_fd = std::strtod(argv[1], NULL);
	size_t message_length = read(read_fd, message, MAX_LINE);
	message[message_length] = '\0';
	close(read_fd);
	string message_string(message);
	size_t space = message_string.find(" ");
	string coefs_file_name = message_string.substr(0, space);
	string data_file_name = message_string.substr(space + 1);
	std::ifstream coefs_file(coefs_file_name);
	std::ifstream data_file(data_file_name);
	std::vector<std::array<double, 3>> coefs;
	bool data_started = false;
	string number = coefs_file_name;
	size_t start_of_name = number.find("classifier_");
	number = number.substr(start_of_name + 11);
	size_t dot = number.find(".");
	number = number.substr(0, dot);
	string fifo_name = string("fifo/voter-fifo") + number;
	int fd = open(fifo_name.c_str(), O_WRONLY | O_RDONLY | O_CREAT, 0666);
	close(fd);

	mkfifo(fifo_name.c_str(), 0666);

	if (!coefs_file || !data_file)
		exit(EXIT_FAILURE);

	while(true)
	{
		std::string b1, b2, bias;

		if (!std::getline(coefs_file, b1, ','))
			break;

		std::getline(coefs_file, b2, ',');
		std::getline(coefs_file, bias, '\n');

		if (!data_started)
		{
			data_started = true;
			continue;
		}

		coefs.push_back({{std::stof(b1), std::stof(b2), std::stof(bias)}});
	}

	data_started = false;
	uint32_t number_of_elements = 0;
	fd = open(fifo_name.c_str(), O_WRONLY);

	while(true)
	{
		std::string length,width;

		if (!std::getline(data_file, length, ','))
			break;

		std::getline(data_file, width, '\n');

		if (!data_started)
		{
			data_started = true;
			continue;
		}

		int32_t detected_class = -1;
		double max_score = -1;

		for (size_t i = 0; i < coefs.size(); ++i)
		{
			double class_score;

			if ((class_score = coefs[i][0] * std::stof(length) + coefs[i][1] * std::stof(width) + coefs[i][2]) > max_score)
			{
				max_score = class_score;
				detected_class = i;
			}
		}

		string data = std::to_string(number_of_elements) + "," + std::to_string(detected_class) + "\n";
		write(fd, data.c_str(), data.length());
		++number_of_elements;
	}

	close(fd);
}