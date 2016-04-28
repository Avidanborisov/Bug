#include "heap.hpp"
#include "math.hpp"

extern void* allocator(size_t pages);

Heap::Header Heap::base = {
    &base, 0
};

Heap::Header* Heap::head = &base;

size_t Heap::Header::units() const {
    return size / sizeof(Header);
}

void* Heap::Header::data() {
    return this + 1;
}

Heap::Header* Heap::Header::meta(void* data) {
    return reinterpret_cast<Header*>(data) - 1;
}

Heap::Header* Heap::expand(size_t bytes) {
    auto blocks = Math::roundUp(bytes, PAGE_SIZE) / PAGE_SIZE;
    auto header = static_cast<Header*>(allocator(blocks));

    if (!header)
        return nullptr;

    header->size = blocks * PAGE_SIZE;
    header->next = head->next;
    head->next = header;

    return head;
}

void* Heap::allocate(size_t size) {
    auto bytes = Math::roundUp(size + sizeof(Header), sizeof(Header));

    auto prev = head;
    auto curr = head->next;

    while (true) {
        if (curr->size >= bytes) {
            if (curr->size == bytes) {
                prev->next = curr->next;
            } else {
                curr->size -= bytes;
                curr += curr->units();
                curr->size = bytes;
            }

            head = prev;
            return curr->data();
        }

        if (curr == head) {
            curr = Heap::expand(bytes); // curr->next now has new block

            if (!curr)
                return nullptr;
        }

        prev = curr;
        curr = curr->next;
    }
}

void Heap::free(void* ptr) {
    if (ptr == nullptr)
        return;

    Header* header = Header::meta(ptr);
    Header* node = head;

    // while header isn't between two free nodes
    while (header <= node || header >= node->next) {
        // if next node wraps around to the start, check if header's between them: either before start or after end
        if (node >= node->next && (header > node || header < node->next))
            break;

        node = node->next;
    }

    // merge the block with it's neighboring nodes (node and node->next)

    if (header + header->units() == node->next) { // if block is linearly before upper node, consume the upper node
        header->size += node->next->size;
        header->next = node->next->next;
    } else { // otherwise link to the upper node
        header->next = node->next;
    }

    if (node + node->units() == header) { // if block is linearly after the lower node, make the lower node consume the block
        node->size += header->size;
        node->next = header->next;
    } else { // otherwise link the block to the node
        node->next = header;
    }

    head = node;
}

void* operator new(size_t size) {
    return Heap::allocate(size);
}

void operator delete(void* p) {
    Heap::free(p);
}

void* operator new[](size_t size) {
    return Heap::allocate(size);
}

void operator delete[](void* p) {
    Heap::free(p);
}
