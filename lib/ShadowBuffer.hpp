#pragma once

namespace gintonic {

class ShadowBuffer
{
public:
	virtual ~ShadowBuffer() noexcept = default;
	virtual void bindForWriting() const noexcept = 0;
	virtual void bindForReading() const noexcept = 0;
};

} // namespace gintonic