
#include <Preferences.h>
#include <nvs_flash.h>

#ifndef NVS_H
#define NVS_H

/**
 *  Manage settings persience, e.g. to keep them during deep sleep.
 */
class Settings {
public:

  // Pass ref to preferences and default values
  Settings(Preferences* preferences, uint32_t default_sleep_time) {
    m_preferences = preferences;
    m_default_sleep_time = default_sleep_time;
  };

  // Init will erase the NVS partition and initialize it.
  void init() {
    nvs_flash_erase(); 
    nvs_flash_init(); 
  };

  // Open storage space for read/write access.
  void begin() {
    m_preferences->begin("esp-iot-display", false);
  };

  // Close preferences for used namespace.
  void end() {
    m_preferences->end();
  };

  // Returns persisted deep sleep time in seconds.
  // If there's no value it returns a default passed during creating this settings.
  uint32_t getSleepTime() {
    return m_preferences->getUInt(m_key_sleep_time, m_default_sleep_time);
  };

  // Persit given deep sleep time.
  void setSleepTime(uint32_t sleep_time) {
    m_preferences->putUInt(m_key_sleep_time, sleep_time);
  };

  // Returns persisted content hash.
  String getContentHash() {
    return m_preferences->getString(m_key_content_hash, "");
  };

  // Persit given deep sleep time.
  void setContentHash(String hash) {
    m_preferences->putString(m_key_content_hash, hash);
  };
  
private:

  // Ref to preferences to handle all actions.
  Preferences* m_preferences;

  // Default value for deep sleep.
  uint32_t m_default_sleep_time;

  // Key to be used to read/write deep sleep settings.
  const char* m_key_sleep_time = "deepsleep";

  // Key to be used to read/write content hash.
  const char* m_key_content_hash = "contenthash";
};

#endif 
