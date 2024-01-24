#include "Tasks.h"
#include <vector>
#include <iostream>

int main() {
	using namespace TaskManager;
	std::vector<std::unique_ptr<Task>> tasks;
	tasks.push_back(std::make_unique<BinaryNammedTask>(BinaryNammedTask(Named("Plus"), [](double fst_arg, double snd_arg) { return fst_arg + snd_arg; }, 3, 7)));
	tasks.push_back(std::make_unique<ClearTasksInContainerTask>(&tasks));
	tasks.push_back(std::make_unique<CountObjectsInContainerTask>(&tasks));
	tasks.push_back(std::make_unique<BinaryNammedTask>(BinaryNammedTask(Named("Minus"), [](double fst_arg, double snd_arg) { return fst_arg - snd_arg; }, 412, 42)));
	std::unique_ptr<BinaryNammedTask> multiplication = std::make_unique<BinaryNammedTask>(BinaryNammedTask(Named("Multiplication"), [](double fst_arg, double snd_arg) { return fst_arg * snd_arg; }, 31, 72));
	tasks.push_back(std::make_unique<AddTaskToContainerTask>(&tasks, std::move(multiplication)));
	tasks.push_back(std::make_unique<CountTasksWithResultInContainerTask>(&tasks));
	tasks.push_back(std::make_unique<BinaryNammedTask>(BinaryNammedTask(Named("Division"), [](double fst_arg, double snd_arg) { return fst_arg / snd_arg; }, 34, 7)));
	tasks.push_back(std::make_unique<CountObjectsTask>());

	std::cout << "Number of objects = " << Object::getNumberOfObjects() << std::endl;

	std::vector<std::string> tasks_info;
	while (!tasks.empty()) {
		auto cur_task = std::move(tasks.back());
		tasks.pop_back();
		cur_task->execute();
		tasks_info.push_back(cur_task->toString());
	}

	for (const auto& info : tasks_info) {
		std::cout << info << std::endl;
	}
	
	tasks.clear();
	tasks_info.clear();
	std::cout << "Alive objects = " << Object::getNumberOfObjects() << std::endl;

	return 0;
}
