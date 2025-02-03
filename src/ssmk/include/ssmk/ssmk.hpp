#ifndef _SSMK_SSMK_HPP_
#define _SSMK_SSMK_HPP_

#include <ssmk/context.hpp>
#include <ssmk/sprite.hpp>

#include <functional>

#define CALLBACK(NAME, ...) \
	private: \
		std::function<void(const Ssmk& __VA_OPT__(,) __VA_ARGS__)> s_##NAME##Callback; \
	public: \
		void NAME##Callback(const std::function<void(const Ssmk& __VA_OPT__(,) __VA_ARGS__)>& callback) { \
			s_##NAME##Callback = callback; \
		} \
		const std::function<void(const Ssmk& __VA_OPT__(,) __VA_ARGS__)>& NAME##Callback() const { \
			return s_##NAME##Callback; \
		} \
		std::function<void(const Ssmk& __VA_OPT__(,) __VA_ARGS__)>& NAME##Callback() { \
			return s_##NAME##Callback; \
		}

namespace sm {

class Ssmk {
public:
	Ssmk(const sm::Context& context = {}): m_context(context) {};

	sm::Context& context = m_context;

public:
	struct CallbackInfo {
		const Ssmk& ssmk;
	};

public:
	void readConfig();
	CALLBACK(configRead)

	void findFiles();
	CALLBACK(
		fileFound
	)
	CALLBACK(
		filesFound
	)

	void readSpriteHeaders();
	CALLBACK(
		imageHeaderRead,
		std::size_t image
	)
	CALLBACK(
		imageHeadersRead,
	)

	void packSprites();
	CALLBACK(
		imagePacked,
		std::size_t imageNo
	)
	CALLBACK(
		imagesPacked
	)

	void makeOutputPng();

	void buildPngChunk();
	CALLBACK(
		pngChunkEntryWritten,
		std::size_t sprite
	)
	CALLBACK(
		pngChuckWritten
	)

	void copySprites();
	CALLBACK(
		spriteRowCopied,
		std::size_t sprite,
		std::size_t row,
		std::size_t pass,
		std::size_t passes
	)
	CALLBACK(
		spriteCopied,
		std::size_t image
	)
	CALLBACK(
		spritesCopied
	)

	void writePng();
	CALLBACK(
		imageRowWritten,
		std::size_t row,
		std::size_t pass,
		std::size_t passes
	)
	CALLBACK(
		pngWritten
	)

public:
	void operator()() {
		readConfig();
		findFiles();
		readSpriteHeaders();
		packSprites();
		makeOutputPng();
		copySprites();
		buildPngChunk();
		writePng();
	}

	static void fillContext(sm::Context& context);

public:
	constexpr static const std::array configFilenames = {
		"ssmk.toml", "sprite.toml", "spritesheet.toml"
	};
	constexpr static const char* chunk_name = "ssMK";

private:
	sm::Context m_context;
};

#undef CALLBACK

}

#endif // !_SSMK_SSMK_HPP_
