#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>


namespace sm {

void Ssmk::readConfig() {
	Ssmk::fillContext(context);

	if (s_configReadCallback)
		s_configReadCallback({*this});
}

}
