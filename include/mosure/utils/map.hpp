#pragma once


namespace mosure::inversify::utils {

    template <typename T>
    struct mp_item {
        using type = T;
    };

    template <typename ...Items>
    struct mp_map : public Items... {
        using Items::
    };

}
