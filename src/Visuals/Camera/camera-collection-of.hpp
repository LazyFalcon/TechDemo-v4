#pragma once
#include "camera-controller.hpp"

namespace camera
{
class Collection
{
private:
    int m_idx {0};
    std::vector<std::shared_ptr<camera::Controller>> m_cameras;

public:
    void add(std::shared_ptr<camera::Controller> toAdd) {
        m_cameras.push_back(toAdd);
    }

    auto begin() {
        return m_cameras.begin();
    }
    auto end() {
        return m_cameras.end();
    }

    void focus() {
        if(not m_cameras.empty())
            m_cameras[m_idx]->focusOn();
    }
    void next() {
        ++m_idx;
        if(m_idx >= m_cameras.size())
            m_idx = 0;
        focus();
    }
    void prev() {
        --m_idx;
        if(m_idx < 0)
            m_idx = m_cameras.size() - 1;
        focus();
    }
    bool hasAny() const {
        return not m_cameras.empty();
    }
    camera::Camera& get() {
        return *m_cameras[m_idx];
    }
    camera::Controller& getController() {
        return *m_cameras[m_idx];
    }
};

}
