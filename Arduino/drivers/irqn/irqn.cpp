#include "irqn.h"
#include <core_debug.h>

/**
 * @brief IRQn auto-assignment bitfield
 * @note Each IRQn is represented by a single bit.
 */
class IRQnManager {
public:
    static constexpr size_t FIRST_IRQN = IRQN_AA_FIRST_IRQN;
    static constexpr size_t AVAILABLE_COUNT = IRQN_AA_AVAILABLE_COUNT;

    /**
     * @brief Check if an IRQn is allocated.
     * @param irqn_index Index of the IRQn in the bitfield.
     * @return True if the IRQn is allocated, false otherwise.
     */
    static inline bool is_irqn_allocated(const size_t irqn_index) {
        return (bitfield_[get_field_index(irqn_index)] & get_field_mask(irqn_index)) != 0;
    }

    /**
     * @brief Allocate or free an IRQn.
     * @param irqn_index Index of the IRQn in the bitfield.
     * @param value True to allocate, false to free.
     */
    static inline void set_irqn_allocation(const size_t irqn_index, const bool value) {
        if (value) {
            bitfield_[get_field_index(irqn_index)] |= get_field_mask(irqn_index);
        } else {
            bitfield_[get_field_index(irqn_index)] &= ~get_field_mask(irqn_index);
        }
    }

    /**
     * @brief Find the next available IRQn.
     * @param irqn_index Reference to store the next available IRQn index.
     * @return True if an available IRQn was found, false otherwise.
     */
    static inline bool find_next_available_irqn(size_t &irqn_index) {
        // 优先使用第一种方式的中断向量
        for (size_t i = 0; i < IRQN_AA_FIRST_GROUP_COUNT; ++i) {
            if (!is_irqn_allocated(i)) {
                irqn_index = i;
                return true;
            }
        }
        // 其次使用第二种方式的中断向量
        for (size_t i = IRQN_AA_SECOND_GROUP_START; i < IRQN_AA_SECOND_GROUP_START + IRQN_AA_SECOND_GROUP_COUNT; ++i) {
            if (!is_irqn_allocated(i)) {
                irqn_index = i;
                return true;
            }
        }
        // 最后使用第三种方式的中断向量
        for (size_t i = IRQN_AA_THIRD_GROUP_START; i < IRQN_AA_THIRD_GROUP_START + IRQN_AA_THIRD_GROUP_COUNT; ++i) {
            if (!is_irqn_allocated(i)) {
                irqn_index = i;
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Translate an IRQn bitfield index to an IRQn number.
     * @param irqn_index Index in the bitfield.
     * @return Corresponding IRQn number.
     */
    static inline IRQn_Type translate_index_to_irqn(const size_t irqn_index) {
        CORE_ASSERT(irqn_index < AVAILABLE_COUNT, "IRQn index out of range");
        return static_cast<IRQn_Type>(irqn_index);
    }

    /**
     * @brief Translate an IRQn number to a bitfield index.
     * @param irqn IRQn number.
     * @return Corresponding index in the bitfield.
     */
    static inline size_t translate_irqn_to_index(const IRQn_Type irqn) {
        const auto irqn_index = static_cast<size_t>(irqn);
        CORE_ASSERT(irqn_index < AVAILABLE_COUNT, "IRQn index out of range");
        return irqn_index;
    }

private:
    static uint8_t bitfield_[AVAILABLE_COUNT / 8];

    static inline size_t get_field_index(const size_t irqn_index) { return irqn_index / 8; }
    static inline uint8_t get_field_mask(const size_t irqn_index) { return 1 << (irqn_index % 8); }
};

uint8_t IRQnManager::bitfield_[IRQnManager::AVAILABLE_COUNT / 8] = {0};

//
// IRQn auto-assignment public API
//
en_result_t irqn_aa_get_(IRQn_Type &irqn)
{
    // get next available IRQn index
    size_t irqn_index;
    if (!IRQnManager::find_next_available_irqn(irqn_index)) {
        // no more IRQn available
        return Error;
    }

    // set IRQn bitfield value to true
    IRQnManager::set_irqn_allocation(irqn_index, true);

    // translate IRQn index to IRQn and done
    irqn = IRQnManager::translate_index_to_irqn(irqn_index);
    return Ok;
}

en_result_t irqn_aa_resign_(IRQn_Type &irqn)
{
    // translate IRQn to IRQn index
    const size_t irqn_index = IRQnManager::translate_irqn_to_index(irqn);

    // set IRQn bitfield value to false
    IRQnManager::set_irqn_allocation(irqn_index, false);

    // done
    return Ok;
}

#ifdef __CORE_DEBUG

en_result_t irqn_aa_get(IRQn_Type &irqn, const char *name)
{
    if (irqn_aa_get_(irqn) != Ok) {
        CORE_ASSERT(false, "IRQn auto-assignment failed for %s\n", name);
    }

    CORE_DEBUG_PRINTF("IRQ%d auto-assigned to %s\n", static_cast<int>(irqn), name);
    return Ok;
}

en_result_t irqn_aa_resign(IRQn_Type &irqn, const char *name)
{
    irqn_aa_resign_(irqn);
    CORE_DEBUG_PRINTF("%s auto-resigned IRQ%d\n", name, static_cast<int>(irqn));
    return Ok;
}

#else

en_result_t irqn_aa_get(IRQn_Type &irqn, const char *name)
{
	return irqn_aa_get_(irqn);
}

en_result_t irqn_aa_resign(IRQn_Type &irqn, const char *name)
{
	return irqn_aa_resign_(irqn);
}
#endif
