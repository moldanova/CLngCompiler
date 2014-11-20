#pragma once

#include "Symbols.h"
#include "Nodes.h"
#include "Value.h"

//  ласс дл€ вычислени€ константных выражений
class Calculator : INodeVisitor
{
public:
	Calculator(Node* expr);
	virtual void OnNode(Node* node);
	virtual void OnNode(ProgramNode* node);
	virtual void OnNode(ExpressionNode* node);	
	virtual void OnNode(ConditionalNode* node);
	virtual void OnNode(BinaryOpNode* node);
	virtual void OnNode(UnaryOpNode* node);	
	virtual void OnNode(ValueNode* node);
	virtual void OnNode(IdentifierNode* node);
	virtual void OnNode(ArrayNode* node);
	virtual void OnNode(FuncCallNode* node);
	virtual void OnNode(CompoundNode* node);
	virtual void OnNode(FunctionNode* node);
	virtual void OnNode(IfNode* node);
	virtual void OnNode(WhileNode* node);
	virtual void OnNode(DoNode* node);
	virtual void OnNode(ForNode* node);
	virtual void OnNode(BreakNode* node);
	virtual void OnNode(ContinueNode* node);
	virtual void OnNode(ReturnNode* node);
	Value result;
};

