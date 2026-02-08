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

static const std::unordered_map<std::string_view, std::string_view>
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

static const std::unordered_map<std::string_view,
                                std::unordered_map<char, std::string_view>>
    latex_accents = {

        {"\\'", {{'a', "á"}, {'e', "é"}, {'i', "í"}, {'o', "ó"}, {'u', "ú"},
                 {'A', "Á"}, {'E', "É"}, {'I', "Í"}, {'O', "Ó"}, {'U', "Ú"},
                 {'c', "ć"}, {'C', "Ć"}, {'n', "ń"}, {'N', "Ń"}, {'y', "ý"},
                 {'Y', "Ý"}, {'s', "ś"}, {'S', "Ś"}, {'z', "ź"}, {'Z', "Ź"}}},

        {"\\`",
         {{'a', "à"},
          {'e', "è"},
          {'i', "ì"},
          {'o', "ò"},
          {'u', "ù"},
          {'A', "À"},
          {'E', "È"},
          {'I', "Ì"},
          {'O', "Ò"},
          {'U', "Ù"}}},

        {"\\^",
         {{'a', "â"},
          {'e', "ê"},
          {'i', "î"},
          {'o', "ô"},
          {'u', "û"},
          {'A', "Â"},
          {'E', "Ê"},
          {'I', "Î"},
          {'O', "Ô"},
          {'U', "Û"}}},

        {"\\\"",
         {{'a', "ä"},
          {'e', "ë"},
          {'i', "ï"},
          {'o', "ö"},
          {'u', "ü"},
          {'A', "Ä"},
          {'E', "Ë"},
          {'I', "Ï"},
          {'O', "Ö"},
          {'U', "Ü"}}},

        {"\\~",
         {{'n', "ñ"},
          {'N', "Ñ"},
          {'a', "ã"},
          {'o', "õ"},
          {'A', "Ã"},
          {'O', "Õ"}}},

        {"\\v", {{'c', "č"}, {'C', "Č"}, {'s', "š"}, {'S', "Š"}, {'z', "ž"},
                 {'Z', "Ž"}, {'r', "ř"}, {'R', "Ř"}, {'n', "ň"}, {'N', "Ň"},
                 {'t', "ť"}, {'T', "Ť"}, {'d', "ď"}, {'D', "Ď"}, {'l', "ľ"},
                 {'L', "Ľ"}, {'e', "ě"}, {'E', "Ě"}, {'g', "ǧ"}, {'G', "Ǧ"}}},

        {"\\H", {{'o', "ő"}, {'O', "Ő"}, {'u', "ű"}, {'U', "Ű"}}},

        {"\\c", {{'c', "ç"}, {'C', "Ç"}}}};

static const std::unordered_map<char, char> latex_escaped = {
    {'&', '&'}, {'%', '%'}, {'$', '$'}, {'#', '#'}, {'_', '_'},
    {'{', '{'}, {'}', '}'}, {'~', '~'}, {'^', '^'}};

static const std::unordered_map<char, std::string> latex_superscripts = {
    {'0', "⁰"}, {'1', "¹"}, {'2', "²"}, {'3', "³"}, {'4', "⁴"}, {'5', "⁵"},
    {'6', "⁶"}, {'7', "⁷"}, {'8', "⁸"}, {'9', "⁹"}, {'+', "⁺"}, {'-', "⁻"},
    {'=', "⁼"}, {'(', "⁽"}, {')', "⁾"}, {'n', "ⁿ"}, {'i', "ⁱ"}};

static const std::unordered_map<char, std::string> latex_subscripts = {
    {'0', "₀"}, {'1', "₁"}, {'2', "₂"}, {'3', "₃"}, {'4', "₄"}, {'5', "₅"},
    {'6', "₆"}, {'7', "₇"}, {'8', "₈"}, {'9', "₉"}, {'+', "₊"}, {'-', "₋"},
    {'=', "₌"}, {'(', "₍"}, {')', "₎"}, {'a', "ₐ"}, {'e', "ₑ"}, {'h', "ₕ"},
    {'k', "ₖ"}, {'l', "ₗ"}, {'m', "ₘ"}, {'n', "ₙ"}, {'o', "ₒ"}, {'p', "ₚ"},
    {'s', "ₛ"}, {'t', "ₜ"}, {'x', "ₓ"}};
}; // namespace Encoding
