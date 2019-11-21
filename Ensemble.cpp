#include "commons.h"

int main(int argc, char** argv)
{
	uint32_t number_of_files;

	for (number_of_files = 0; ;)
	{
		mkdir("fifo", 0777);
		string weight_file = string(argv[2]) + string("/classifier_") + std::to_string(number_of_files) + string(".csv");
		char* const weight_file_name = const_cast<char*>(weight_file.c_str());
		string data_file = string(argv[1]) + string("/dataset.csv");
		char* const data_file_name = const_cast<char* const>(data_file.c_str());		
		std::ifstream file(weight_file_name);

		if (!file)
			break;

		int pipe_fd[2];

		if (pipe(pipe_fd) < 0)
		{
			perror("Error on creating unnamed pipe!");
			exit(EXIT_FAILURE);
		}

		if (fork() == 0)
		{
			close(pipe_fd[1]);
			char* const exec_file = const_cast<char* const>("Linear.out");
			std::string pipe_destination_string = std::to_string(pipe_fd[0]);
			char* const pipe_destination = const_cast<char*>(pipe_destination_string.c_str());
			char* const child_argv[] = {exec_file, pipe_destination, data_file_name, NULL};
			execv(child_argv[0], child_argv);
		}
		else
		{
			close(pipe_fd[0]);
			write(pipe_fd[1], weight_file_name, weight_file.length()+1);
			close(pipe_fd[1]);
			++number_of_files;
		}
	}

	for (uint32_t i = 0; i < number_of_files; ++i)
		wait(NULL);


	if (fork() == 0)
	{
		char* const voter_argv[] = {const_cast<char* const>((string(argv[1]) + string("/labels.csv")).c_str()), NULL};
		execv("Voter.out", voter_argv);
	}
	else
	{
		wait(NULL);
		const char* fifo_name = "fifo/ensemble-fifo";
		int fd = open(fifo_name, O_WRONLY | O_RDONLY | O_CREAT, 0666);
		mkfifo(fifo_name, 0666);
		std::ifstream fifo_file(fifo_name);
		string label_file_name = string(argv[1]) + "/labels.csv";
		std::ifstream label_file(label_file_name);
		std::map<uint32_t, uint32_t> result;
		int all = 0;
		int match = 0;
		bool data_started = false;

		while (true)
		{
			string number, detected_class, label;
			std::getline(label_file, label, '\n');

			if (!data_started)
			{
				data_started = true;
				continue;
			}

			if (!std::getline(fifo_file, number, ','))
				break;

			std::getline(fifo_file, detected_class, '\n');
			++all;

			if (label == detected_class)
				++match;
		}

		unlink(fifo_name);
		rmdir("fifo");
		printf("Accuracy: %.2f", 100.0 * match / all);
	}
}