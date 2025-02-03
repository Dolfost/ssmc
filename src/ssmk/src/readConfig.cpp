#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

namespace sm {

void Ssmk::readConfig() {
	Ssmk::fillContext(context);

	if (m_config_read_callback)
		m_config_read_callback({*this});
}

}
