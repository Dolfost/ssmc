#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

#include <filesystem>

namespace sm {

#define SSMK_ADD_SPRITE(P) \
	Sprite* sprite = new Sprite;  \
	sprite->setPath(P);  \
	context.im.sprites.push_back(sprite);  \
	if (m_file_found_callback)  \
		m_file_found_callback(*this); \

void ssmk::find_files() {
	for (auto ptr: context.im.sprites) {
		delete ptr;
	}
	context.im.sprites.clear();
	for (const auto& in: context.input.files) {
		if (std::filesystem::is_directory(in)) {
			for (const auto& entry: std::filesystem::recursive_directory_iterator(in)) {
				if (entry.is_regular_file() and entry.path().extension() == ".png") {
					SSMK_ADD_SPRITE(entry.path())
				}
			}
		} else if (std::filesystem::is_regular_file(in) and in.extension() == ".png") {
			SSMK_ADD_SPRITE(in)
		}
	}
	if (context.im.sprites.empty())
		SM_EX_THROW(Error, NoSpritesFound)
	if (m_files_found_callback)
		m_files_found_callback(*this);
}

#undef SSMK_ADD_SPRITE

}
