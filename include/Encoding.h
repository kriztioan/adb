/**
 *  @file   Encoding.h
 *  @brief  Encoding Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#pragma once

#include <array>
#include <charconv>
#include <cstring>
#include <sstream>
#include <string>
#include <string_view>

#include "Pool.h"

namespace Encoding {
char *URLDecodeInplace(char *str);

std::string URLEncode(std::string_view sv);
std::string_view LaTeXDecode(std::string_view sv, Pool &pool);
std::string HTMLEncode(std::string_view sv);
std::string HTML2XML(std::string_view sv);

static inline const std::unordered_map<std::string_view, std::string_view>
    latex_symbols = {{"\\sim", "~"},
                     {"\\alpha", "α"},
                     {"\\beta", "β"},
                     {"\\gamma", "γ"},
                     {"\\delta", "δ"},
                     {"\\epsilon", "ε"},
                     {"\\zeta", "ζ"},
                     {"\\eta", "η"},
                     {"\\theta", "θ"},
                     {"\\iota", "ι"},
                     {"\\kappa", "κ"},
                     {"\\lambda", "λ"},
                     {"\\mu", "μ"},
                     {"\\nu", "ν"},
                     {"\\xi", "ξ"},
                     {"\\pi", "π"},
                     {"\\rho", "ρ"},
                     {"\\sigma", "σ"},
                     {"\\tau", "τ"},
                     {"\\upsilon", "υ"},
                     {"\\phi", "φ"},
                     {"\\chi", "χ"},
                     {"\\psi", "ψ"},
                     {"\\omega", "ω"},
                     {"\\Gamma", "Γ"},
                     {"\\Delta", "Δ"},
                     {"\\Theta", "Θ"},
                     {"\\Lambda", "Λ"},
                     {"\\Xi", "Ξ"},
                     {"\\Pi", "Π"},
                     {"\\Sigma", "Σ"},
                     {"\\Phi", "Φ"},
                     {"\\Psi", "Ψ"},
                     {"\\Omega", "Ω"},
                     {"\\ae", "æ"},
                     {"\\AE", "Æ"},
                     {"\\oe", "œ"},
                     {"\\OE", "Œ"},
                     {"\\ss", "ß"},
                     {"\\aa", "å"},
                     {"\\AA", "Å"},
                     {"\\o", "ø"},
                     {"\\O", "Ø"},
                     {"\\l", "ł"},
                     {"\\L", "Ł"},
                     {"\\i", "ı"},
                     {"\\j", "ȷ"},
                     {"\\dh", "ð"},
                     {"\\DH", "Ð"},
                     {"\\th", "þ"},
                     {"\\TH", "Þ"},
                     {"\\times", "×"},
                     {"\\div", "÷"},
                     {"\\pm", "±"},
                     {"\\cdot", "·"},
                     {"\\ast", "∗"},
                     {"\\leq", "≤"},
                     {"\\geq", "≥"},
                     {"\\neq", "≠"},
                     {"\\approx", "≈"},
                     {"\\equiv", "≡"},
                     {"\\infty", "∞"},
                     {"\\partial", "∂"},
                     {"\\nabla", "∇"},
                     {"\\forall", "∀"},
                     {"\\exists", "∃"},
                     {"\\in", "∈"},
                     {"\\notin", "∉"},
                     {"\\subset", "⊂"},
                     {"\\subseteq", "⊆"},
                     {"\\supset", "⊃"},
                     {"\\supseteq", "⊇"},
                     {"\\cup", "∪"},
                     {"\\cap", "∩"},
                     {"\\emptyset", "∅"},
                     {"\\rightarrow", "→"},
                     {"\\leftarrow", "←"},
                     {"\\Rightarrow", "⇒"},
                     {"\\Leftarrow", "⇐"},
                     {"\\leftrightarrow", "↔"},
                     {"\\textdagger", "†"},
                     {"\\textdaggerdbl", "‡"},
                     {"\\mapsto", "↦"},
                     {"\\textdegree", "°"},
                     {"\\textcopyright", "©"},
                     {"\\textregistered", "®"},
                     {"\\texttrademark", "™"},
                     {"\\textellipsis", "…"},
                     {"\\textemdash", "—"},
                     {"\\textendash", "–"}};

static inline const std::unordered_map<char, std::string_view> latex_accents = {
    {'\'', "\u0301"}, {'`', "\u0300"}, {'^', "\u0302"}, {'"', "\u0308"},
    {'~', "\u0303"},  {'=', "\u0304"}, {'.', "\u0307"}, {'c', "\u0327"},
    {'v', "\u030C"},  {'H', "\u030B"}};

static inline const std::unordered_map<char, char> latex_escaped = {
    {'&', '&'}, {'%', '%'}, {'$', '$'}, {'#', '#'}, {'_', '_'},
    {'{', '{'}, {'}', '}'}, {'~', '~'}, {'^', '^'}};

static inline const std::unordered_map<char, std::string> latex_superscripts = {
    {'0', "⁰"}, {'1', "¹"}, {'2', "²"}, {'3', "³"}, {'4', "⁴"}, {'5', "⁵"},
    {'6', "⁶"}, {'7', "⁷"}, {'8', "⁸"}, {'9', "⁹"}, {'+', "⁺"}, {'-', "⁻"},
    {'=', "⁼"}, {'(', "⁽"}, {')', "⁾"}, {'n', "ⁿ"}, {'i', "ⁱ"}};

static inline const std::unordered_map<char, std::string> latex_subscripts = {
    {'0', "₀"}, {'1', "₁"}, {'2', "₂"}, {'3', "₃"}, {'4', "₄"}, {'5', "₅"},
    {'6', "₆"}, {'7', "₇"}, {'8', "₈"}, {'9', "₉"}, {'+', "₊"}, {'-', "₋"},
    {'=', "₌"}, {'(', "₍"}, {')', "₎"}, {'a', "ₐ"}, {'e', "ₑ"}, {'h', "ₕ"},
    {'k', "ₖ"}, {'l', "ₗ"}, {'m', "ₘ"}, {'n', "ₙ"}, {'o', "ₒ"}, {'p', "ₚ"},
    {'s', "ₛ"}, {'t', "ₜ"}, {'x', "ₓ"}};
}; // namespace Encoding
