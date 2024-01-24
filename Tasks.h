#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace TaskManager {

	class Object {
	public:
		Object() {
			++sm_number_of_objects;
		}

		Object(Object const&) {
			++sm_number_of_objects;
		}

		Object(Object&&) noexcept {
			++sm_number_of_objects;
		}

		Object& operator=(Object&&) noexcept {
			++sm_number_of_objects;
			return *this;
		}

		Object& operator=(Object const&) {
			++sm_number_of_objects;
			return *this;
		}

		virtual std::string toString() const = 0;

		virtual ~Object() {
			--sm_number_of_objects;
		}

		static uint64_t getNumberOfObjects() {
			return sm_number_of_objects;
		}

	private:
		static inline uint64_t sm_number_of_objects = 0;
	};

	class Named : public Object {
	public:
		explicit Named(std::string name) : m_name(std::move(name)) {}

		std::string toString() const override {
			return std::string("Object name ") + m_name;
		}

		virtual ~Named() = default;

	private:
		std::string const m_name;
	};

	class Task : public Object {
	public:
		void virtual execute() = 0;

		virtual ~Task() = default;

		virtual bool checkIfWithResultTask() const = 0;

	private:
	};

	class BinaryNammedTask : public virtual Task, Named {
	public:
		BinaryNammedTask(Named name_of_task, std::function<double(double, double)> const& binary_operation, double first_argument, double second_argument)
			: Named(name_of_task)
			, m_binary_operation(binary_operation)
			, m_first_argument(first_argument)
			, m_second_argument(second_argument)
		{}

		void execute() override {
			m_result = m_binary_operation(m_first_argument, m_second_argument);
		}

		std::string toString() const override {
			std::string info = Named::toString() + std::string(", binary operation with arguments = (") + std::to_string(m_first_argument) + std::string(", ") + std::to_string(m_second_argument) + std::string(")");
			if (m_result) {
				info += std::string(" Result = ") + std::to_string(*m_result);
			}
			return info;
		}

		bool checkIfWithResultTask() const override {
			return true;
		}

	private:
		std::function<double(double, double)> m_binary_operation;
		double m_first_argument;
		double m_second_argument;
		std::optional<double> m_result;
	};

	class AddTaskToContainerTask : public Task {
	public:
		AddTaskToContainerTask(std::vector<std::unique_ptr<Task>>* container_with_tasks_ptr, std::unique_ptr<Task>&& task)
			: m_container_with_tasks_ptr(container_with_tasks_ptr)
			, m_task(std::move(task))
			, m_task_info(m_task->toString())
		{
			if (!m_task) {
				throw std::invalid_argument("Empty task in AddTaskToContainerTask constructor.");
			}
		}

		void execute() override {
			if (!m_task) {
				throw std::runtime_error("Try to execute AddTaskToContainerTask twice.");
			}
			m_container_with_tasks_ptr->push_back(std::move(m_task));
		}

		std::string toString() const override {
			std::string info = std::string("AddTaskToContainerTask adds task [") + m_task_info + std::string("]");
			return info;
		}

		bool checkIfWithResultTask() const override {
			return false;
		}

	private:
		std::vector<std::unique_ptr<Task>>* m_container_with_tasks_ptr;
		std::unique_ptr<Task> m_task;
		const std::string m_task_info;
	};

	class CountObjectsInContainerTask : public Task {
	public:
		CountObjectsInContainerTask(std::vector<std::unique_ptr<Task>>* container_with_tasks_ptr)
			: m_container_with_tasks_ptr(container_with_tasks_ptr)
		{}

		void execute() override {
			m_count_of_objects_result = m_container_with_tasks_ptr->size();
		}

		std::string toString() const override {
			std::string info = std::string("CountObjectsInContainerTask");
			if (!m_count_of_objects_result) {
				info += " wasn't running yet.";
			}
			else {
				info += " result = " + std::to_string(*m_count_of_objects_result);
			}
			return info;
		}

		bool checkIfWithResultTask() const override {
			return true;
		}

	private:
		std::vector<std::unique_ptr<Task>>* m_container_with_tasks_ptr;
		std::optional<int64_t> m_count_of_objects_result;
	};

	class CountTasksWithResultInContainerTask : public Task {
	public:
		CountTasksWithResultInContainerTask(std::vector<std::unique_ptr<Task>>* container_with_tasks_ptr)
			: m_container_with_tasks_ptr(container_with_tasks_ptr)
		{}

		void execute() override {
			m_count_of_tasks_with_result = 0;
			for (const auto& task : *m_container_with_tasks_ptr) {
				if (task->checkIfWithResultTask()) {
					++*m_count_of_tasks_with_result;
				}
			}
		}

		std::string toString() const override {
			std::string info = std::string("CountTasksWithResultInContainerTask");
			if (!m_count_of_tasks_with_result) {
				info += " wasn't running yet.";
			}
			else {
				info += " result = " + std::to_string(*m_count_of_tasks_with_result);
			}
			return info;
		}

		bool checkIfWithResultTask() const override {
			return true;
		}

	private:
		std::vector<std::unique_ptr<Task>>* m_container_with_tasks_ptr;
		std::optional<int64_t> m_count_of_tasks_with_result;
	};

	class ClearTasksInContainerTask : public Task {
	public:
		ClearTasksInContainerTask(std::vector<std::unique_ptr<Task>>* container_with_tasks_ptr)
			: m_container_with_tasks_ptr(container_with_tasks_ptr)
		{}

		void execute() override {
			m_container_with_tasks_ptr->clear();
		}

		std::string toString() const override {
			std::string info = std::string("ClearTasksInContainerTask");
			return info;
		}

		bool checkIfWithResultTask() const override {
			return false;
		}

	private:
		std::vector<std::unique_ptr<Task>>* m_container_with_tasks_ptr;
	};

	class CountObjectsTask : public Task {
	public:

		void execute() override {
			m_count_of_objects = Object::getNumberOfObjects();
		}

		std::string toString() const override {
			std::string info = std::string("CountObjectsTask");
			if (!m_count_of_objects) {
				info += " wasn't running yet.";
			}
			else {
				info += " result = " + std::to_string(*m_count_of_objects);
			}
			return info;
		}

		bool checkIfWithResultTask() const override {
			return true;
		}

	private:
		std::optional<uint64_t> m_count_of_objects;
	};
}
