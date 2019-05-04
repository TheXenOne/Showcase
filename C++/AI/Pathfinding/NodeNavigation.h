#pragma once

#include "GraphNode.h"

#include <DirectXMath.h>

class NodeNavigation : public GraphNode
{
public:
	NodeNavigation() {}
	NodeNavigation(int index) : m_Position(DirectX::XMFLOAT3())
	{
		m_Index = index;
	}
	NodeNavigation(int index, DirectX::XMFLOAT3 position) : m_Position(position)
	{
		m_Index = index;
	}
	virtual ~NodeNavigation() {}

	virtual void SaveNode(std::ostream& a_os) const
	{
		a_os << "Index: " << m_Index << " PosX: " << m_Position.x << " PosZ: " << m_Position.z << std::endl;
	}

	const DirectX::XMFLOAT3& GetPositionF3() const { return m_Position; }
	DirectX::XMVECTOR GetPosition() const { return DirectX::XMLoadFloat3(&m_Position); }
	void SetPosition(const DirectX::XMFLOAT3& a_position) { m_Position = a_position; }
	void SetPosition(const DirectX::XMVECTOR& a_position) { DirectX::XMStoreFloat3(&m_Position, a_position); }
protected:
	DirectX::XMFLOAT3 m_Position;
};