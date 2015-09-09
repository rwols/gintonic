#include "component.hpp"
#include "entity.hpp"

namespace gintonic {

component::component(component&& other) BOOST_NOEXCEPT_OR_NOTHROW
: m_ents(std::move(other.m_ents))
{
	/* Empty on purpose. */
}

component& component::operator = (component&& other)
BOOST_NOEXCEPT_OR_NOTHROW
{
	m_ents = std::move(other.m_ents);
	return *this;
}

component::~component() BOOST_NOEXCEPT_OR_NOTHROW
{
	/* Empty on purpose. */
}

void component::entity_died_handler(entity& e)
{
	for (auto i = m_ents.begin(); i != m_ents.end(); ++i)
	{
		if (*i == &e)
		{
			m_ents.erase(i);
			return;
		}
	}
}

} // namespace gintonic