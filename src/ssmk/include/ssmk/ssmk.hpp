#ifndef _SSMK_SSMK_HPP_
#define _SSMK_SSMK_HPP_

#include <ssmk/context.hpp>

#include <functional>

#define CALLBACK(NAME) \
	private: \
		std::function<void(const sm::Context&)> s_##NAME##Callback; \
	public: \
		void NAME##Callback(const std::function<void(const sm::Context&)>& callback) { \
			s_##NAME##Callback = callback; \
		} \
		const std::function<void(const sm::Context&)>& NAME##Callback() const { \
			return s_##NAME##Callback; \
		} \
		std::function<void(const sm::Context&)>& NAME##Callback() { \
			return s_##NAME##Callback; \
		}

namespace sm {

class Ssmk {
public:
	Ssmk(const sm::Context& context = {}): s_context(context) {};

	sm::Context& context = s_context;

public:
	void readConfig();
	CALLBACK(configRead)

	void findFiles() {};
	CALLBACK(fileFound)
	CALLBACK(filesFound)

	void readImageHeaders() {};
	CALLBACK(imageHeaderRead)
	CALLBACK(imageHeadersRead)

	void packImages() {};
	CALLBACK(imagePacked)
	CALLBACK(imagesPacked)

	void writeImages() {};
	CALLBACK(imageWritten)
	CALLBACK(imagesWritten)

	void writeSheetInfo() {};
	CALLBACK(sheetEntryWritten)
	CALLBACK(sheetInfoWritten)

public:
	void operator()() {
		readConfig();
		findFiles();
		readImageHeaders();
		packImages();
		writeImages();
		writeSheetInfo();
	}

private:
	sm::Context s_context;
};

#undef CALLBACK

}

#endif // !_SSMK_SSMK_HPP_
