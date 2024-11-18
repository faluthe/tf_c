float get_latency(void *net_channel_info, int flow)
{
    void **vtable = *(void ***)net_channel_info;
    float (*func)(void *, int) = vtable[10];

    return func(net_channel_info, flow);
}