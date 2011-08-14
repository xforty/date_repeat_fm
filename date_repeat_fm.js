// $Id$ 

$(document).ready(function(){
  $('.date_repeat_fm-view-all').click(
    function(event) {
      $(this).siblings('.date_repeat_fm-listing').toggle("fast",
        function () {
          if (this.style.display == 'block') {
            $(this).siblings('.date_repeat_fm-view-all').children('a').html('Hide all dates');
          }
          else {
            $(this).siblings('.date_repeat_fm-view-all').children('a').html('View all dates');
          }
        }
      );
      return false;
    }
  );
});