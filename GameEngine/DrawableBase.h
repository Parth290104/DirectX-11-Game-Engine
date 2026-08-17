#pragma once
#include "Drawable.h"
#include "IndexBuffer.h"

template<class T>
class DrawableBase : public Drawable
{
protected:
	bool IsStaticInitialized() const noexcept
	{
		return !staticBinds.empty();
	}

	static void AddStaticBind(std::unique_ptr<Bindable> bindable) noexcept(!IS_DEBUG)
	{
		assert("Must use AddStaticIndexBuffer to bind index buffer" && typeid(*bindable) != typeid(IndexBuffer));
			staticBinds.push_back(std::move(bindable));
	}

	void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> indexBuffer) noexcept(!IS_DEBUG)
	{
		assert(pIndexBuffer == nullptr);
		pIndexBuffer = indexBuffer.get();
		staticBinds.push_back(std::move(indexBuffer));
	}

	void SetIndexFromStatic() noexcept(!IS_DEBUG)
	{
		assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);

		for (const auto& bindable : staticBinds)
		{
			if (const auto p = dynamic_cast<IndexBuffer*>(bindable.get()))
			{
				pIndexBuffer = p;
				return;
			}
		}

		assert("Failed to find index buffer in static binds" && pIndexBuffer != nullptr);
	}

private:
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept
	{
		return staticBinds;
	}

private:
	static std::vector<std::unique_ptr<Bindable>> staticBinds;
};

template<class T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::staticBinds;
