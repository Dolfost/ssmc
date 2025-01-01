#ifndef _SSMK_SSMK_HPP_
#define _SSMK_SSMK_HPP_

#include <ssmk/context.hpp>
#include <ssmk/sprite.hpp>

#include <functional>

#define CALLBACK(NAME, CALLFIELDS) \
	public: \
		struct Callback##NAME##Info: CallbackInfo { \
			CALLFIELDS \
		}; \
	private: \
		std::function<void(const Callback##NAME##Info&)> s_##NAME##Callback; \
	public: \
		void NAME##Callback(const std::function<void(const Callback##NAME##Info&)>& callback) { \
			s_##NAME##Callback = callback; \
		} \
		const std::function<void(const Callback##NAME##Info&)>& NAME##Callback() const { \
			return s_##NAME##Callback; \
		} \
		std::function<void(const Callback##NAME##Info&)>& NAME##Callback() { \
			return s_##NAME##Callback; \
		}

namespace sm {

class Ssmk {
public:
	Ssmk(const sm::Context& context = {}): s_context(context) {};

	sm::Context& context = s_context;

public:
	struct CallbackInfo {
		const Ssmk& ssmk;
	};

public:
	void readConfig();
	CALLBACK(configRead,)

	void findFiles();
	CALLBACK(
		fileFound, 
		const Sprite& image;
	)
	CALLBACK(
		filesFound, 
		const std::vector<Sprite>& images;
	)

	void readImageHeaders() {};
	CALLBACK(
		imageHeaderRead,
		const Sprite& image;
		std::size_t headerNo;
		std::size_t headerCount;
	)
	CALLBACK(
	imageHeadersRead,
		const std::vector<Sprite>& images;
	)

	void packImages() {};
	CALLBACK(
		imagePacked,
		const Sprite& image;
		std::size_t imageCount;
		std::size_t imageNo;
	)
	CALLBACK(
		imagesPacked,
		const std::vector<Sprite>& images;
	)

	void writeImages() {};
	CALLBACK(
		imageRowWritten,
		const Sprite& image;
		std::size_t rowCount;
		std::size_t rowNo;
	)
	CALLBACK(
		imageWritten,
		const Sprite& image;
		std::size_t imageCount;
		std::size_t imageNo;
	)
	CALLBACK(
		imagesWritten,
		const std::vector<Sprite>& images;
	)

	void writeSheetInfo() {};
	CALLBACK(
		sheetEntryWritten,
		const Sprite& image;
		std::size_t spriteCount;
		std::size_t spriteNo;
	)
	CALLBACK(
		sheetInfoWritten,
		const std::vector<Sprite>& images;
	)

public:
	void operator()() {
		readConfig();
		findFiles();
		readImageHeaders();
		packImages();
		writeImages();
		writeSheetInfo();
	}

	static void fillContext(sm::Context& context);

public:
	constexpr static const std::array configFilenames = {
		"ssmk.toml", "sprite.toml", "spritesheet.toml"
	};

private:
	sm::Context s_context;
};

#undef CALLBACK

}

#endif // !_SSMK_SSMK_HPP_
