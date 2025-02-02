#pragma once

#include <Kernel/FileSystem/InodeIdentifier.h>
#include <Kernel/TTY/TTY.h>

class MasterPTY;

class SlavePTY final : public TTY {
public:
    virtual ~SlavePTY() override;

    void on_master_write(const u8*, ssize_t);
    unsigned index() const { return m_index; }

    InodeIdentifier devpts_inode_id() const { return m_devpts_inode_id; }
    void set_devpts_inode_id(InodeIdentifier inode_id) { m_devpts_inode_id = inode_id; }

private:
    // ^TTY
    virtual String tty_name() const override;
    virtual ssize_t on_tty_write(const u8*, ssize_t) override;

    // ^CharacterDevice
    virtual bool can_read(FileDescription&) const override;
    virtual ssize_t read(FileDescription&, u8*, ssize_t) override;
    virtual bool can_write(FileDescription&) const override;
    virtual const char* class_name() const override { return "SlavePTY"; }
    virtual void close() override;

    friend class MasterPTY;
    SlavePTY(MasterPTY&, unsigned index);

    RefPtr<MasterPTY> m_master;
    unsigned m_index;
    InodeIdentifier m_devpts_inode_id;
    String m_tty_name;
};
