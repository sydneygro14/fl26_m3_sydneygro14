// ======================================================================
// REFERENCE ONLY (version Oct. 6, 2026)
// Read this file to understand the provided interface. Do NOT add it to
// your build, and do NOT replace the provided header with it. Your project
// must keep using the original header; the required API must stay unchanged.
// ======================================================================

// QT NOTE: Big picture. Resettable is an INTERFACE: a class that only promises
// "I can be reset" and says nothing about how. WorkspaceWidget, SearchWidget,
// TermInspectorWidget and ContextWidget all inherit from it, so MainWindow can
// reset every one of them through this single type (polymorphism, M3 requirement).
// Comments labeled "QT NOTE" are explanations only; the code is unchanged.
#pragma once

namespace aiws::gui {

/**
 * @brief Defines the common reset operation for major M3 GUI components.
 *
 * A reset restores a component to the state it has when the application first
 * starts. Classes that implement this interface provide their own component-
 * specific reset behavior.
 */
// QT NOTE: This is plain C++, not a Qt class: no QObject, no Q_OBJECT, because it
// needs no signals or slots. It is abstract (it has a pure virtual function), so
// you cannot create a Resettable object directly; you inherit from it.
class Resettable {
public:
    // QT NOTE: A virtual destructor makes sure that deleting a derived object through a
    // Resettable* runs the derived class's destructor too. "= default" means
    // "use the compiler-generated version".
    virtual ~Resettable() = default;

    /**
     * @brief Restores the component to its initial application state.
     */
    // QT NOTE: Virtual = the version that runs is chosen by the object's real type.
    // "= 0" makes it PURE virtual: every class that inherits Resettable MUST provide
    // its own reset(), written in the derived class as: void reset() override;
    // Each widget decides what "initial application state" means for itself.
    // Example: MainWindow can loop over Resettable* pointers and call reset() on each,
    // without knowing which widget it is talking to.
    virtual void reset() = 0;
};

}  // namespace aiws::gui
