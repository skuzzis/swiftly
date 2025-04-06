#include <public/irecipientfilter.h>

class CSingleRecipientFilter : public IRecipientFilter
{
public:
    CSingleRecipientFilter(int iRecipient, bool bReliable = true, bool bInitMessage = false) : m_bReliable(bReliable), m_bInitMessage(bInitMessage), m_iRecipient(iRecipient) {}

    ~CSingleRecipientFilter() override {}

    bool IsReliable(void) const { return m_bReliable; }

    bool IsInitMessage(void) const override { return m_bInitMessage; }

    NetChannelBufType_t GetNetworkBufType(void) const override { return m_bReliable ? BUF_RELIABLE : BUF_UNRELIABLE; }

    int GetRecipientCount(void) const override { return 1; }

    CPlayerSlot GetRecipientIndex(int slot) const override { return CPlayerSlot(m_iRecipient); }

private:
    bool m_bReliable;
    bool m_bInitMessage;
    int m_iRecipient;
};
