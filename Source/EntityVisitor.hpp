#pragma once

#include "ForwardDeclarations.hpp"
#include <memory>

namespace gintonic {

class EntityVisitor
{
public:

	std::shared_ptr<Entity> root;

	EntityVisitor() = default;

	EntityVisitor(std::shared_ptr<Entity> root);
	
	virtual ~EntityVisitor() = default;

	void execute();

protected:

	inline std::size_t currentDepth() const noexcept
	{
		return mDepth;
	}

private:

	std::size_t mDepth;
	
	bool mContinue;

	inline virtual bool onStart() { return true; }
	
	virtual bool onVisit(std::shared_ptr<Entity> entity) = 0;
	
	inline virtual void onFinish() {}

	void visit(std::shared_ptr<Entity>);
};

} // namespace gintonic