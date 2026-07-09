/**
 * Pure theme preference resolution used by doxyYoda header boot logic.
 * Keep in sync with the localStorage / prefers-color-scheme rules in header.html.
 *
 * @param {string|null|undefined} stored  value of localStorage 'doxyYoda-theme'
 * @param {boolean} prefersDark           matchMedia('(prefers-color-scheme: dark)')
 * @returns {'dark'|'light'}
 */
function resolveThemePreference(stored, prefersDark) {
  if (stored === "dark") return "dark";
  if (stored === "light") return "light";
  return prefersDark ? "dark" : "light";
}

/**
 * Next theme after a toggle click.
 * @param {boolean} currentlyDark
 * @returns {'dark'|'light'}
 */
function nextThemeAfterToggle(currentlyDark) {
  return currentlyDark ? "light" : "dark";
}

if (typeof module !== "undefined" && module.exports) {
  module.exports = { resolveThemePreference, nextThemeAfterToggle };
}
if (typeof window !== "undefined") {
  window.doxyYodaThemePref = { resolveThemePreference, nextThemeAfterToggle };
}
