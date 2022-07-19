#pragma once
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <string>
#include <utility>

template <typename Type>
class SingleLinkedList {
  // Узел списка
  struct Node {
	Node() = default;
	Node(const Type& val, Node* next) : value(val), next_node(next) {}
	Type value;
	Node* next_node = nullptr;
  };

  //*********************************** ITERATOR *****************************************
  template <typename ValueType>
  class BasicIterator {
	friend class SingleLinkedList;

	explicit BasicIterator(Node* node) : node_{node} {
	}

   public:
	using iterator_category = std::forward_iterator_tag;
	using value_type = Type;
	using difference_type = std::ptrdiff_t;
	using pointer = ValueType*;
	using reference = ValueType&;

	BasicIterator() = default;

	BasicIterator(const BasicIterator<Type>& other) noexcept : node_{other.node_} {
	}

	BasicIterator& operator=(const BasicIterator& rhs) = default;

	[[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
	  return node_ == rhs.node_;
	}

	[[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
	  return !(*this == rhs);
	}

	[[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
	  return node_ == rhs.node_;
	}

	[[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
	  return !(*this == rhs);
	}

	BasicIterator& operator++() noexcept {
	  node_ = node_->next_node;
	  return *this;
	}

	BasicIterator operator++(int) noexcept {
	  Node* ret = node_;
	  node_ = node_->next_node;
	  return BasicIterator(ret);
	}

	[[nodiscard]] reference operator*() const noexcept {
	  return node_->value;
	}

	[[nodiscard]] pointer operator->() const noexcept {
	  return &node_->value;
	}

   private:
	Node* node_ = nullptr;
  };
  //********************************** END ITERATOR **************************************

 public:
  using value_type = Type;
  using reference = value_type&;
  using const_reference = const value_type&;
  using Iterator = BasicIterator<Type>;
  using ConstIterator = BasicIterator<const Type>;

  [[nodiscard]] Iterator begin() noexcept { return Iterator(head_.next_node); }

  [[nodiscard]] Iterator end() noexcept {
	return Iterator(nullptr);
  }

  [[nodiscard]] ConstIterator begin() const noexcept {
	return ConstIterator(head_.next_node);
  }

  [[nodiscard]] ConstIterator end() const noexcept {
	return ConstIterator(nullptr);
  }

  [[nodiscard]] ConstIterator cbegin() const noexcept {
	return ConstIterator(head_.next_node);
  }

  [[nodiscard]] ConstIterator cend() const noexcept {
	return ConstIterator(nullptr);
  }

  [[nodiscard]] Iterator before_begin() noexcept { return Iterator(&head_); }

  [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
	return ConstIterator(const_cast<Node*>(&head_));
  }

  [[nodiscard]] ConstIterator before_begin() const noexcept {
	return ConstIterator(&head_);
  }

 public:
  SingleLinkedList() : head_{}, size_{0} {}

  SingleLinkedList(std::initializer_list<Type> values) {
	SingleLinkedList temp;
	for (auto it = std::rbegin(values); it != std::rend(values); ++it) {
	  temp.PushFront(*it);
	}
	swap(temp);
  }

  SingleLinkedList(const SingleLinkedList& other) {
	SingleLinkedList t1;
	for (const auto& el : other) {
	  t1.PushFront(el);
	}
	SingleLinkedList t2;
	for (const auto& el : t1) {
	  t2.PushFront(el);
	}
	swap(t2);
  }

  SingleLinkedList& operator=(const SingleLinkedList& rhs) {
	if (this != &rhs) {
	  SingleLinkedList temp(rhs);
	  swap(temp);
	}
	return *this;
  }

  void swap(SingleLinkedList& other) noexcept {
	if (this != &other) {
	  std::swap(head_.next_node, other.head_.next_node);
	  std::swap(size_, other.size_);
	}
  }

  ~SingleLinkedList() { Clear(); }

  [[nodiscard]] size_t GetSize() const noexcept { return size_; }

  [[nodiscard]] bool IsEmpty() const noexcept { return size_ == 0; }

  void PushFront(const Type& el) {
	head_.next_node = new Node(el, head_.next_node);
	++size_;
  }

  void Clear() noexcept {
	while (head_.next_node) {
	  Node* del = head_.next_node;
	  head_.next_node = del->next_node;
	  delete del;
	  --size_;
	}
  }

  Iterator InsertAfter(ConstIterator pos, const Type& value) {
	if (pos == cend()) {
	  return end();
	}
	Node* insert_node = new Node(value, pos.node_->next_node);
	pos.node_->next_node = insert_node;
	++size_;
	return Iterator{insert_node};
  }

  void PopFront() noexcept {
	Node* del_node = head_.next_node;
	if (!del_node) {
	  return;
	}
	head_.next_node = del_node->next_node;
	delete del_node;
	--size_;
  }

  Iterator EraseAfter(ConstIterator pos) noexcept {
	if (!pos.node_) {
	  return end();
	}
	Node* del_node = pos.node_->next_node;
	if (!del_node) {
	  return end();
	}
	pos.node_->next_node = del_node->next_node;
	delete del_node;
	--size_;
	return Iterator{pos.node_->next_node};
  }

 private:
  // Фиктивный узел
  Node head_;
  size_t size_;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
  lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
  if (lhs.GetSize() != rhs.GetSize()) {
	return false;
  }
  return std::equal(std::begin(lhs), std::end(lhs), std::begin(rhs));
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
  return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
  return std::lexicographical_compare(std::cbegin(lhs), std::cend(lhs), std::cbegin(rhs),
									  std::end(rhs));
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
  return (lhs == rhs) || (lhs < rhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
  return !(lhs <= rhs);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
  return !(lhs < rhs);
}
