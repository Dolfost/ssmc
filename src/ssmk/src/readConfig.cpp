#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

namespace sm {

void ssmk::read_config() {
	ssmk::fill_context(context);

	if (m_config_read_callback)
		m_config_read_callback({*this});
}

}
