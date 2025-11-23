#pragma once

namespace CompanionTakeAll::Events
{
    class MenuOpenCloseEventSink : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
    {
    public:
        virtual RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent& a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_source) override;
    };

    extern MenuOpenCloseEventSink MenuEventSink;
}
