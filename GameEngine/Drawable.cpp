#include "Drawable.h"
#include "Graphics.h"
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"
#include <cassert>
#include <typeinfo>

void Drawable::Draw(Graphics& graphicsObject) const noexcept (!IS_DEBUG)
{
	for (auto& bindable : binds)
		bindable->Bind(graphicsObject);

	graphicsObject.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::unique_ptr<Bindable> bindable) noexcept
{
	assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bindable) != typeid(IndexBuffer));
	binds.push_back(bindable);
}

void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> indexBuffer) noexcept
{
	assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
	pIndexBuffer = indexBuffer.get();
	binds.push_back(std::move(indexBuffer));
}
