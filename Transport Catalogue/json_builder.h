#pragma once

#include<stack>
#include<string>
#include<memory>
#include"json.h"

namespace json {
	class DictItemContext;
	class KeyItemContext;
	class ArrayItemContext;
	class Builder
	{
	public:
		Node MakeNode(Node::Value value);
		void AddNode(Node node);
		KeyItemContext Key(std::string key);
		Builder& Value(Node::Value value);
		DictItemContext StartDict();
		ArrayItemContext StartArray();
		Builder& EndDict();
		Builder& EndArray();
		Node Build();
	private:
		Node root_;
		std::vector<std::unique_ptr<Node>> stackk_;
	};
	class DictItemContext {
	public:
		DictItemContext(Builder& builder);
		KeyItemContext Key(std::string key);
		Builder& EndDict();
	private:
		Builder& builder_;
	};

	class KeyItemContext {
	public:
		KeyItemContext(Builder& builder);
		DictItemContext Value(Node::Value value);
		ArrayItemContext StartArray();
		DictItemContext StartDict();
	private:
		Builder& builder_;
	};

	class ArrayItemContext {
	public:
		ArrayItemContext(Builder& builder);
		ArrayItemContext Value(Node::Value value);
		DictItemContext StartDict();
		ArrayItemContext StartArray();
		Builder& EndArray();
	private:
		Builder& builder_;
	};
}//namespace json
