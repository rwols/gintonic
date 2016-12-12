#pragma once

#include "ForwardDeclarations.hpp"
#include <memory>

namespace gintonic {

/**
 * @brief Base class for inheritance. You use this class
 * to traverse children of entities in a depth-first manner.
 * Derive from it and implement the EntityVisitor::onVisit method.
 */
class EntityVisitor
{
public:

	/**
	 * @brief Start the depth-first search with this Entity.
	 */
	std::shared_ptr<Entity> root;

	/**
	 * @brief Default constructor sets the root Entity to null.
	 */
	EntityVisitor() = default;

	/**
	 * @brief Construct an EntityVisitor with the given Entity
	 * as root for the depth-first search. The search is started
	 * when the method EntityVisitor::execute is called.
	 * @param root The root Entity to start the depth-first search
	 * from.
	 */
	EntityVisitor(std::shared_ptr<Entity> root);
	
	/**
	 * @brief Default destructor.
	 */
	virtual ~EntityVisitor() = default;

	/**
	 * @brief Run the depth-first search starting at the
	 * root Entity.
	 */
	void execute();

protected:

	/**
	 * @brief Get the current depth. Can be useful when you
	 * are implementing the EntityVisitor::onVisit method.
	 * @return The current depth.
	 */
	inline std::size_t currentDepth() const noexcept
	{
		return mDepth;
	}

private:

	std::size_t mDepth;
	
	bool mContinue;

	/**
	 * @brief This gets called at the start of the search.
	 * The method should return true when the search should continue.
	 * It should return false when the search should be aborted.
	 * The default implementation always returns true. Note that
	 * the base class EntityVisitor also checks if the root Entity
	 * is not null. If it is, the search is aborted no matter what
	 * this method returns.
	 * @return True if the search should commence, false if the search
	 * should be aborted.
	 */
	inline virtual bool onStart() { return true; }
	
	/**
	 * @brief This method gets called everytime a new Entity is encountered
	 * in the search tree. You should return true if the search should continue
	 * and return false if the search should be aborted.
	 * @param entity The current Entity that is visited.
	 * @return True if the search should continue, false if the search
	 * should be aborted.
	 */
	virtual bool onVisit(std::shared_ptr<Entity> entity) = 0;
	
	/**
	 * @brief This method gets called at the end of the search, just before
	 * the EntityVisitor::execute method returns. It always gets called, even
	 * if EntityVisitor::onStart returns false, and even if EntityVisitor::onVisit
	 * returns false at some point.
	 */
	inline virtual void onFinish() {}

	void visit(std::shared_ptr<Entity>);
};

} // namespace gintonic