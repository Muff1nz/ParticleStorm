#include <vector>

template<typename  T>
class LazyVector {
public:
	void Add(T item) {
		if (currentIndex < buffer.size()) {
			buffer[currentIndex] = item;
		} else {
			buffer.push_back(item);
		}
		++currentIndex;
	}

	void GrowOne() {
		++currentIndex;
	}

	int Size() const {
		return currentIndex;
	}

	int TrueSize() const {
		return buffer.size();
	}

	void Clear() {
		currentIndex = 0;
	}

	T operator[](T index) const {
		return buffer[index];
	}

	T* Get(int index) {
		return &buffer[index];
	}

	T GetValue(int index) const {
		return buffer[index];
	}

private:
	std::vector<T> buffer;
	int currentIndex = 0;
};