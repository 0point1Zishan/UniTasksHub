package com.example.musicwiki.model

import java.io.Serializable

// ==================== Genre List ====================
data class Genre(
    val toptags: Toptags
)

data class Toptags(
    val tag: List<Tag>
)

data class Tag(
    val count: String,
    val name: String,
    val url: String
) : Serializable


// ==================== Genre Detail ====================
data class GenreDetail(
    val tag: GenreInfo
)

data class GenreInfo(
    val name: String,
    val reach: Int,
    val total: Int,
    val wiki: Wiki?
)

data class Wiki(
    val content: String,
    val summary: String,
    val published: String,
)