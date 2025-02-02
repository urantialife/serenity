#include <Kernel/Process.h>
#include <Kernel/Thread.h>
#include <Kernel/VM/MemoryManager.h>
#include <Kernel/VM/PageDirectory.h>

static const u32 userspace_range_base = 0x01000000;
static const u32 kernelspace_range_base = 0xc0000000;

PageDirectory::PageDirectory(PhysicalAddress paddr)
    : m_range_allocator(VirtualAddress(0xc0000000), 0x3f000000)
{
    m_directory_page = PhysicalPage::create(paddr, true, false);
}

PageDirectory::PageDirectory(const RangeAllocator* parent_range_allocator)
    : m_range_allocator(parent_range_allocator ? RangeAllocator(*parent_range_allocator) : RangeAllocator(VirtualAddress(userspace_range_base), kernelspace_range_base - userspace_range_base))
{
    MM.populate_page_directory(*this);
}

PageDirectory::~PageDirectory()
{
#ifdef MM_DEBUG
    dbgprintf("MM: ~PageDirectory K%x\n", this);
#endif
}

void PageDirectory::flush(VirtualAddress vaddr)
{
#ifdef MM_DEBUG
    dbgprintf("MM: Flush page L%x\n", vaddr.get());
#endif
    if (!current)
        return;
    if (this == &MM.kernel_page_directory() || &current->process().page_directory() == this)
        MM.flush_tlb(vaddr);
}
