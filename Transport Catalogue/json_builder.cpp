#include "json_builder.h"
namespace json {
	Node Builder::MakeNode(Node::Value value)
	{
		Node node;
		if (std::holds_alternative<bool>(value)) {
			bool val = std::get<bool>(value);
			node = Node(val);
		}
		else if (std::holds_alternative<int>(value)) {
			int val = std::get<int>(value);
			node = Node(val);
		}
		else if (std::holds_alternative<double>(value)) {
			double val = std::get<double>(value);
			node = Node(val);
		}
		else if (std::holds_alternative<std::string>(value)) {
			std::string str = std::get<std::string>(value);
			node = Node(std::move(str));
		}
		else if (std::holds_alternative<Array>(value)) {
			Array arr = std::get<Array>(value);
			node = Node(std::move(arr));
		}
		else if (std::holds_alternative<Dict>(value)) {
			Dict dict = std::get<Dict>(value);
			node = Node(std::move(dict));
		}
		else {
			node = Node();
		}
		return node;
	}

	void Builder::AddNode(Node node)
	{
		if (stackk_.empty()) {
			if (!root_.IsNull()) {
				throw std::logic_error("Root already added.");
			}
			root_ = node;
			return;
		}
		else {
			if (!stackk_.back()->IsString() && !stackk_.back()->IsArray()) {
				throw std::logic_error("Unable to create node.");
			}
			if (stackk_.back()->IsArray()) {
				Array arr = stackk_.back()->AsArray();
				arr.emplace_back(node);
				stackk_.pop_back();
				auto arr_ptr = std::make_unique<Node>(arr);
				stackk_.emplace_back(std::move(arr_ptr));
				return;
			}
			if (stackk_.back()->IsString()) {
				std::string str = stackk_.back()->AsString();
				stackk_.pop_back();
				if (stackk_.back()->IsDict()) {
					Dict dict = stackk_.back()->AsDict();
					dict.emplace(std::move(str), node);
					stackk_.pop_back();
					auto dict_ptr = std::make_unique<Node>(dict);
					stackk_.emplace_back(std::move(dict_ptr));
				}
				return;
			}
		}
	}

	KeyItemContext Builder::Key(std::string key)
	{
		if (stackk_.empty()) {
			throw std::logic_error("Unable to create key.");
		}
		auto key_ptr = std::make_unique<Node>(key);
		if (stackk_.back()->IsDict()) {
			stackk_.emplace_back(std::move(key_ptr));
		}
		return *this;
	}

	Builder& Builder::Value(Node::Value value)
	{
		AddNode(MakeNode(value));
		return *this;
	}

	DictItemContext Builder::StartDict()
	{
		stackk_.emplace_back(std::move(std::make_unique<Node>(Dict())));
		return *this;
	}

	ArrayItemContext Builder::StartArray()
	{
		stackk_.emplace_back(std::move(std::make_unique<Node>(Array())));
		return *this;
	}

	Builder& Builder::EndDict()
	{
		if (stackk_.empty()) {
			throw std::logic_error("Unable to close, because dictionary not opened.");
		}
		Node node = *stackk_.back();
		if (!node.IsDict()) {
			throw std::logic_error("Obj isnt dictionary.");
		}
		stackk_.pop_back();
		AddNode(node);
		return *this;
	}

	Builder& Builder::EndArray()
	{
		if (stackk_.empty()) {
			throw std::logic_error("Unable to close, because array not opened.");
		}
		Node node = *stackk_.back();
		if (!node.IsArray()) {
			throw std::logic_error("Obj isnt array.");
		}
		stackk_.pop_back();
		AddNode(node);
		return *this;
	}

	Node Builder::Build()
	{
		if (root_.IsNull()) {
			throw std::logic_error("Empty file.");
		}
		if (!stackk_.empty()) {
			throw std::logic_error("Invalid file.");
		}
		return root_;
	}
	DictItemContext::DictItemContext(Builder& builder) : builder_(builder) {}
	KeyItemContext DictItemContext::Key(std::string key)
	{
		return builder_.Key(key);
	}
	Builder& DictItemContext::EndDict()
	{
		return builder_.EndDict();
	}
	KeyItemContext::KeyItemContext(Builder& builder) : builder_(builder) {}

	DictItemContext KeyItemContext::Value(Node::Value value)
	{
		return DictItemContext(builder_.Value(value));
	}

	ArrayItemContext KeyItemContext::StartArray()
	{
		return builder_.StartArray();
	}

	DictItemContext KeyItemContext::StartDict()
	{
		return builder_.StartDict();
	}

	ArrayItemContext::ArrayItemContext(Builder& builder) : builder_(builder) {}
	ArrayItemContext ArrayItemContext::Value(Node::Value value)
	{
		return ArrayItemContext(builder_.Value(value));
	}
	DictItemContext ArrayItemContext::StartDict()
	{
		return builder_.StartDict();
	}
	ArrayItemContext ArrayItemContext::StartArray()
	{
		return builder_.StartArray();
	}
	Builder& ArrayItemContext::EndArray()
	{
		return builder_.EndArray();
	}
}//namespace json
