#pragma once


namespace mosure::inversify {

    template <typename Interface>
    class Cache {
        public:
            Interface& getValue();
            void setValue(Interface& value);

        private:
            Interface& value_;
    };

}
