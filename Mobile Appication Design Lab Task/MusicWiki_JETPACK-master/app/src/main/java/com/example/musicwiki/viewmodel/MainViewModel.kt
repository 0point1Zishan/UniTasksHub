package com.example.musicwiki.viewmodel

import android.util.Log
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.example.musicwiki.model.GenreInfo
import com.example.musicwiki.model.Tag
import com.example.musicwiki.services.DataService
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.launch
import retrofit2.Call
import retrofit2.Callback
import retrofit2.Response

class MainViewModel : ViewModel() {

    private val apiService = DataService.lastFmService
    private val apiKey = DataService.getApiKey()

    // Genre List এর জন্য
    private val _genres = MutableStateFlow<List<Tag>>(emptyList())
    val genres: StateFlow<List<Tag>> = _genres

    // Genre Detail এর জন্য
    private val _genreDetail = MutableStateFlow<GenreInfo?>(null)
    val genreDetail: StateFlow<GenreInfo?> = _genreDetail

    // সব Genre লোড করা
    fun getAllGenres() {
        viewModelScope.launch {
            apiService.getGenres(
                method = "tag.getTopTags",
                apiKey = apiKey,
                format = "json"
            ).enqueue(object : Callback<com.example.musicwiki.model.Genre> {

                override fun onResponse(
                    call: Call<com.example.musicwiki.model.Genre>,
                    response: Response<com.example.musicwiki.model.Genre>
                ) {
                    if (response.isSuccessful) {
                        response.body()?.toptags?.tag?.let {
                            _genres.value = it
                            Log.d("MainViewModel", "Genres Loaded: ${it.size}")
                        }
                    } else {
                        Log.d("MainViewModel", "Error: ${response.code()}")
                    }
                }

                override fun onFailure(call: Call<com.example.musicwiki.model.Genre>, t: Throwable) {
                    Log.d("MainViewModel", "Failed: ${t.message}")
                }
            })
        }
    }

    // নির্দিষ্ট Genre এর বিস্তারিত তথ্য লোড করা
    fun getGenreInfo(tagName: String) {
        viewModelScope.launch {
            apiService.getTagInfo(
                method = "tag.getinfo",
                tagName = tagName,
                apiKey = apiKey,
                format = "json"
            ).enqueue(object : Callback<com.example.musicwiki.model.GenreDetail> {

                override fun onResponse(
                    call: Call<com.example.musicwiki.model.GenreDetail>,
                    response: Response<com.example.musicwiki.model.GenreDetail>
                ) {
                    if (response.isSuccessful) {
                        _genreDetail.value = response.body()?.tag
                        Log.d("MainViewModel", "Genre Detail Loaded")
                    } else {
                        Log.d("MainViewModel", "Error: ${response.code()}")
                    }
                }

                override fun onFailure(call: Call<com.example.musicwiki.model.GenreDetail>, t: Throwable) {
                    Log.d("MainViewModel", "Failed: ${t.message}")
                }
            })
        }
    }
}